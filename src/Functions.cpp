#include "Functions.hpp"

bpo::options_description Functions::addProgramDescription()
{
	bpo::options_description description(HELP);
	description.add_options()
	("help,h",                                   "Shows the help message")
	("version,v",                                "Shows the version of the program")
	("input,i",       bpo::value<std::string>(), "A path to the input image file")
	("output,o",      bpo::value<std::string>(), "A path to the output image file");

	return description;
}

CharactersLuminosity Functions::calculateCharactersLuminosity()
{
	CharactersLuminosity charactersLuminosity;
	for (size_t character = 0; character < options.characters.chars.size(); ++character) {
		std::string letter;
		letter += options.characters.chars[character];

		Magick::Image letterImage = Functions::getLetterImage(letter);
		Luma luma = Functions::calculateLuma(letterImage);

		charactersLuminosity.push_back(luma);
	}

	return charactersLuminosity;
}

Luma Functions::calculateLuma(const Magick::Image &image, unsigned long xOffset, unsigned long yOffset)
{
	Luma luma;
	for (unsigned int row = 0; row < options.regions.rows; ++row) {
		for (unsigned int column = 0; column < options.regions.columns; ++column) {
			double luminosity = Functions::getLuminosity(Functions::getAverageColor(image, xOffset + options.characters.width / options.regions.columns * column, yOffset + options.characters.height / options.regions.rows * row, options.characters.width / options.regions.columns, options.characters.height / options.regions.rows)) / 65536;
			luma.push_back(luminosity);
		}
	}

	return luma;
}

Luma Functions::calculateLuma(const Magick::Image &image)
{
	return Functions::calculateLuma(image, 0, 0);
}

CharacterList Functions::convertStringToChars(const std::string &chars)
{
	CharacterList characterList;
	for (size_t character = 0; character < chars.size(); ++character) {
		characterList.push_back(chars[character]);
	}

	return characterList;
}

std::string Functions::convertCharsToString(const CharacterList &chars)
{
	std::string output;
	for (size_t character = 0; character < chars.size(); ++character) {
		output += chars[character];
	}

	return output;
}

Magick::Color Functions::getAverageColor(const Magick::Image &image, unsigned int xstart, unsigned int ystart, unsigned int width, unsigned int height)
{
	Color meanColor;
	unsigned long size = width * height;
	for (unsigned int column = xstart; column - xstart < width; ++column) {
		for (unsigned int row = ystart; row - ystart < height; ++row) {
			Magick::Color pixelColor = image.pixelColor(column, row);
			meanColor.red += pixelColor.quantumRed();
			meanColor.green += pixelColor.quantumGreen();
			meanColor.blue += pixelColor.quantumBlue();
		}
	}

	return Magick::Color(meanColor.red / size, meanColor.green / size, meanColor.blue / size);
}

Magick::Color Functions::getAverageColor(const Magick::Image &image)
{
	return getAverageColor(image, FULL_IMAGE(image));
}

Magick::Image Functions::getLetterImage(std::string letter)
{
	Magick::Image letterImage(Magick::Geometry(options.characters.width, options.characters.height), "black");
	letterImage.fillColor("white");
	letterImage.strokeAntiAlias(options.characters.antialiasing);
	letterImage.fontPointsize(options.characters.size);
	letterImage.font("notomono.ttf");
	letterImage.draw(Magick::DrawableText(0, options.characters.size, letter));

	return letterImage;
}

double Functions::getLuminosity(const Magick::Color &color)
{
	return options.colorsWeights.red   * color.quantumRed()   +
		   options.colorsWeights.green * color.quantumGreen() +
		   options.colorsWeights.blue  * color.quantumBlue();
}

Magick::Image Functions::getASCII(const Magick::Image &image)
{
	CharactersLuminosity charactersLuminosity = Functions::calculateCharactersLuminosity();
	Magick::Geometry geometry(image.columns(), image.rows());
	Magick::Image newImage(geometry, options.image.backgroundColor);

	newImage.strokeAntiAlias(false);
	newImage.fontPointsize(options.characters.size);
	newImage.font("notomono.ttf");

	unsigned int columns = image.columns() / options.characters.width;
	unsigned int rows    = image.rows()    / options.characters.height;
	unsigned int yOffset = options.characters.size;


	for (unsigned int x = 0; x < columns; ++x) {
		for (unsigned int y = 0; y < rows; ++y) {
			Luma luma = Functions::calculateLuma(image, x * options.characters.width, y * options.characters.height);

			std::string letter;
			letter += Functions::matchLetter(charactersLuminosity, luma);

			Magick::Color newColor = Functions::getAverageColor(image, x * options.characters.width, y * options.characters.height, options.characters.width, options.characters.height);
			newImage.fillColor(newColor);
			newImage.draw(Magick::DrawableText(x * options.characters.width, y * options.characters.height + yOffset, letter));
		}
	}

	return newImage;
}

bpo::variables_map Functions::getVariablesMap(bpo::options_description description, int argc, char *argv[])
{
	bpo::variables_map variablesMap;
	bpo::positional_options_description positionalOptionsDescription;
	positionalOptionsDescription.add("input", 1);
	positionalOptionsDescription.add("output", 1);

	try {
		bpo::store(bpo::command_line_parser(argc, argv).options(description).positional(positionalOptionsDescription).run(), variablesMap);
		bpo::notify(variablesMap);
	} catch (std::exception &exception) {
		PRINT("Command option error: " << exception.what());
	} catch (...) {
		PRINT("Unknown error");
	}

	return variablesMap;
}

void Functions::loadOptions()
{
	try {
		if (!bfs::exists(OPTIONS_INI)) {
			options.save(OPTIONS_INI);
		} else {
			options.load(OPTIONS_INI);
		}
	} catch (std::exception &exception) {
		PRINT(exception.what() << "; loading default settings");
	}
}


wchar_t Functions::matchLetter(CharactersLuminosity charactersLuminosity, const Luma &luma)
{
	double minDistance = 0;
	size_t index = 0;
	for (size_t character = 0; character < options.characters.chars.size(); ++character) {
		double distance = 0;
		for (size_t region = 0; region < 9; ++region) {
			distance += (luma[region] - charactersLuminosity[character][region]) * (luma[region] - charactersLuminosity[character][region]);
		}

		if (distance < minDistance or minDistance == 0) {
			minDistance = distance;
			index = character;
		}
	}

	return options.characters.chars[index];
}

void Functions::readImage(Magick::Image &image, const std::string &inputPath)
{
	try {
		image.read(inputPath);
	} catch (Magick::Exception &exception) {
		PRINT(exception.what());
	}
}
