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

ImageData Functions::analyzeImage(Magick::Image &image, const CharactersData &charactersData)
{
	unsigned int columns  = image.columns() / options.characters.width;
	unsigned int rows     = image.rows()    / options.characters.height;

	_LogInfo("Analyzing the image: " + std::to_string(columns) + "x" + std::to_string(rows) + "...")
	ImageData imageData(columns, rows);
	for (unsigned int column = 0; column < columns; ++column) {
		for (unsigned int row = 0; row < rows; ++row) {
			ImageCell imageCell = Functions::analyzeRegion(image, column, row, charactersData);
			imageData.setCell(column, row, imageCell);
		}
	}

	_LogInfo("Succesfully analyzed the image");

	return imageData;
}

ImageCell Functions::analyzeRegion(Magick::Image &image, unsigned int column, unsigned int row, bool analyzeColor)
{
	Magick::Geometry geometry(image.columns(), image.rows());
	Magick::Quantum *pixels = image.getPixels(0, 0, geometry.width(), geometry.height());

	unsigned int channels = image.channels();
	unsigned long offset = channels * (row * geometry.width() * options.characters.height + column * options.characters.width);

	Color averageColor = {0, 0, 0};
	LumaGrid lumaGrid;
	lumaGrid.resize(options.regions.columns, LumaVector(options.regions.rows, 0));
	for (unsigned int x = 0; x < options.characters.width; ++x) {
		for (unsigned int y = 0; y < options.characters.height; ++y) {
			unsigned int c = x / options.regions.columns;
			unsigned int r = y / options.regions.rows;
			unsigned long position = offset + channels * (y * geometry.width() + x);
			Color color = {pixels[position], pixels[position + 1], pixels[position + 2]};
			lumaGrid[c][r] += Functions::getLuminosity(color);

			if (analyzeColor) {
				averageColor.red   += color.red;
				averageColor.green += color.green;
				averageColor.blue  += color.blue;
			}
		}
	}

	for (unsigned int c = 0; c < options.regions.columns; ++c) {
		for (unsigned int r = 0; r < options.regions.rows; ++r) {
			lumaGrid[c][r] /= options.regionSize() * DEPTH;
		}
	}

	if (analyzeColor) {
		averageColor.red   /= options.cellSize();
		averageColor.green /= options.cellSize();
		averageColor.blue  /= options.cellSize();
		double lumaModifier = sqrt(Functions::getLuminosity(averageColor) / DEPTH);
		Magick::Color color = Magick::Color(averageColor.red / lumaModifier, averageColor.green / lumaModifier, averageColor.blue / lumaModifier);
		return ImageCell(lumaGrid, color, ' ');
	} else {
		return ImageCell(lumaGrid, "white", ' ');
	}
}

ImageCell Functions::analyzeRegion(Magick::Image &image, unsigned int column, unsigned int row, const CharactersData &charactersData)
{
	ImageCell imageCell = Functions::analyzeRegion(image, column, row, true);
	LumaGrid lumaGrid = imageCell.getLumaGrid();
	wchar_t letter = Functions::matchLetter(charactersData, lumaGrid);
	imageCell.setLetter(letter);

	return imageCell;
}

CharactersData Functions::calculateCharactersData()
{
	_LogInfo("Analyzing characters data...");
	CharactersData charactersData;
	Magick::Color color = Magick::Color(1.0f, 1.0f, 1.0f);
	for (size_t character = 0; character < options.characters.chars.size(); ++character) {
		wchar_t letter = options.characters.chars[character];
		Magick::Image letterImage = Functions::getLetterImage(letter);
		ImageCell characterData = Functions::analyzeRegion(letterImage, 0, 0);
		characterData.setLetter(letter);
		charactersData[letter] = characterData;
	}

	_LogInfo("Characters data analyzed successfully");

	return charactersData;
}

CharactersList Functions::convertStringToChars(const std::string &chars)
{
	CharactersList CharactersList;
	for (size_t character = 0; character < chars.size(); ++character) {
		CharactersList.push_back(chars[character]);
	}

	return CharactersList;
}

std::string Functions::convertCharsToString(const CharactersList &chars)
{
	std::string output;
	for (size_t character = 0; character < chars.size(); ++character) {
		output += chars[character];
	}

	return output;
}

Magick::Image Functions::drawOutput(const ImageData &imageData)
{
	Magick::Image newImage(Magick::Geometry(imageData.columns * options.characters.width, imageData.rows * options.characters.height), options.image.backgroundColor);
	newImage.strokeAntiAlias(options.characters.antialiasing);
	newImage.fontPointsize(options.characters.size);
	newImage.font("notomono.ttf");

	unsigned int yOffset = options.characters.size;
	for (unsigned int column = 0; column < imageData.columns; ++column) {
		for (unsigned int row = 0; row < imageData.rows; ++row) {
			std::string outputLetter;
			outputLetter += imageData[column][row].second;
			newImage.fillColor(imageData[column][row].first);
			newImage.draw(Magick::DrawableText(column * options.characters.width, row * options.characters.height + yOffset, outputLetter));
		}
	}

	return newImage;
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

Magick::Image Functions::makeASCII(Magick::Image &image)
{
	CharactersData charactersData = Functions::calculateCharactersData();
	ImageData imageData = Functions::analyzeImage(image, charactersData);
	Magick::Image outputImage = Functions::drawOutput(imageData);

	_LogInfo("Succesfully converted the image");

	return outputImage;
}

wchar_t Functions::matchLetter(const CharactersData &charactersData, const LumaGrid &luma)
{
	double minDistance = 0;
	wchar_t letter = ' ';

	for (const auto &character : charactersData) {
		double distance = 0;
		CharacterData characterData = character.second;
		for (unsigned int row = 0; row < options.regions.rows; ++row) {
			for (unsigned int column = 0; column < options.regions.columns; ++column) {
				distance += square(luma[column][row] - characterData[column][row]);
			}
		}

		if (distance < minDistance or minDistance == 0) {
			minDistance = distance;
			letter = character.first;
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

void Functions::writeImage(Magick::Image &image, const std::string &outputPath)
{
	try {
		image.write(outputPath);
		_LogInfo("Succesfully saved " + outputPath + " file");
	} catch (Magick::Exception &exception) {
		_LogError(exception.what());
	}
}
