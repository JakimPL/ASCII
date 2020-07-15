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

CharactersData Functions::calculateCharactersData()
{
	CharactersData charactersData;
	for (size_t character = 0; character < options.characters.chars.size(); ++character) {
		ImageData imageData(1, 1);

		wchar_t letter = options.characters.chars[character];

		Magick::Image letterImage = Functions::getLetterImage(letter);
		imageData.calculateLuma(letterImage);

		imageData.setLetter(letter);
		charactersData.push_back(imageData);
	}

	return charactersData;
}

LumaGrid Functions::calculateLuma(const Magick::Image &image, unsigned long xOffset, unsigned long yOffset)
{
	LumaGrid luma;
	for (unsigned int row = 0; row < options.regions.rows; ++row) {
		LumaVector lumaVector;
		for (unsigned int column = 0; column < options.regions.columns; ++column) {
			double luminosity = Functions::getLuminosity(Functions::getAverageColor(image, xOffset + options.characters.width / options.regions.columns * column, yOffset + options.characters.height / options.regions.rows * row, options.characters.width / options.regions.columns, options.characters.height / options.regions.rows)) / 65536;
			lumaVector.push_back(luminosity);
		}

		luma.push_back(lumaVector);
	}

	return luma;
}

LumaGrid Functions::calculateLuma(const Magick::Image &image)
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

Magick::Image Functions::getASCII(const Magick::Image &image)
{
	CharactersData charactersData = Functions::calculateCharactersData();
	Magick::Geometry geometry(image.columns(), image.rows());
	Magick::Image newImage(geometry, options.image.backgroundColor);

	newImage.strokeAntiAlias(false);
	newImage.fontPointsize(options.characters.size);
	newImage.font("notomono.ttf");

	unsigned int columns = image.columns() / options.characters.width;
	unsigned int rows    = image.rows()    / options.characters.height;
	unsigned int yOffset = options.characters.size;

	ImageData imageData(columns, rows);
	for (unsigned int x = 0; x < columns; ++x) {
		for (unsigned int y = 0; y < rows; ++y) {
			LumaGrid luma = Functions::calculateLuma(image, x * options.characters.width, y * options.characters.height);

			std::string letter;
			letter += Functions::matchLetter(charactersData, luma);

			Magick::Color newColor = Functions::getAverageColor(image, x * options.characters.width, y * options.characters.height, options.characters.width, options.characters.height);
			newImage.fillColor(newColor);
			newImage.draw(Magick::DrawableText(x * options.characters.width, y * options.characters.height + yOffset, letter));
		}
	}

	_LogInfo("Succesfully converted an image")
	return newImage;
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

Magick::Image Functions::getLetterImage(wchar_t letter)
{
	Magick::Image letterImage(Magick::Geometry(options.characters.width, options.characters.height), "black");
	std::string letterString;
	letterString += letter;

	letterImage.fillColor("white");
	letterImage.strokeAntiAlias(options.characters.antialiasing);
	letterImage.fontPointsize(options.characters.size);
	letterImage.font("notomono.ttf");
	letterImage.draw(Magick::DrawableText(0, options.characters.size, letterString));

	return letterImage;
}

double Functions::getLuminosity(const Magick::Color &color)
{
	return options.colorsWeights.red   * color.quantumRed()   +
		   options.colorsWeights.green * color.quantumGreen() +
		   options.colorsWeights.blue  * color.quantumBlue();
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
		_LogError("Command option error: " + STR(exception.what()));
	} catch (...) {
		_LogError("Unknown error");
	}

	return variablesMap;
}

void Functions::loadOptions()
{
	try {
		if (!bfs::exists(OPTIONS_INI)) {
			options.save(OPTIONS_INI);
			_LogInfo("Options saved to " + STR(OPTIONS_INI) + " file");
		} else {
			options.load(OPTIONS_INI);
			_LogInfo("Options loaded from " + STR(OPTIONS_INI) + " file");
		}
	} catch (std::exception &exception) {
		_LogError(STR(exception.what()) + "; loading default settings");
	}
}


wchar_t Functions::matchLetter(const CharactersData &charactersData, const LumaGrid &luma)
{
	double minDistance = 0;
	wchar_t letter = ' ';

	for (ImageData character : charactersData) {
		double distance = 0;
		for (unsigned int row = 0; row < options.regions.rows; ++row) {
			for (unsigned int column = 0; column < options.regions.columns; ++column) {
				distance += square(luma[column][row] - character.getLuma(column, row));
			}
		}

		if (distance < minDistance or minDistance == 0) {
			minDistance = distance;
			letter = character.getLetter();
		}
	}

	return letter;
}

int Functions::parseProgramArguments(int argc, char *argv[], std::string &inputPath, std::string &outputPath)
{
	bpo::options_description description = Functions::addProgramDescription();
	bpo::variables_map variablesMap = Functions::getVariablesMap(description, argc, argv);

	bool exit = false;
	if (variablesMap.count("help")) {
		PRINT(description);
		exit = true;
	}

	if (variablesMap.count("version")) {
		PRINT("Version: " << VERSION);
		exit = true;
	}

	if (variablesMap.count("input")) {
		inputPath = variablesMap["input"].as<std::string>();
		std::fstream file(inputPath);
		if (!file.good()) {
			_LogError("Failed to load the input file!");
			return -1;
		}

		if (variablesMap.count("output")) {
			outputPath = variablesMap["output"].as<std::string>();
			if (outputPath == inputPath) {
				_LogError("Output path cannot be the same as input path!");
				return -1;
			}
		} else {
			_LogError("No output path specified!");
			return -1;
		}
	} else {
		PRINT(description << std::endl << "Version: " << VERSION);
		return 1;
	}

	if (exit) {
		return 1;
	}

	return 0;
}

void Functions::readImage(Magick::Image &image, const std::string &inputPath)
{
	try {
		image.read(inputPath);
		_LogInfo("Succesfully read " + inputPath + " file");
	} catch (Magick::Exception &exception) {
		_LogError(exception.what());
	}
}
