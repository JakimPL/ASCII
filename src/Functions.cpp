#include "Functions.hpp"

bpo::options_description Functions::addProgramDescription()
{
	bpo::options_description description(HELP);
	description.add_options()
	("help,h",                                 "Shows the help message")
	("version,v",                              "Shows the version of the program")
	("invert,n",                               "Invert the background (black is default)")
	("input,i",     bpo::value<std::string>(), "A path to the input image file")
	("output,o",    bpo::value<std::string>(), "A path to the output image file")
	("threshold,t", bpo::value<double>(),      "The threshold of the algorithm (a value from 0 to 1)");

	return description;
}

ImageData Functions::analyzeImage(Magick::Image &image, const CharactersData &charactersData)
{
	unsigned int columns  = image.columns() / options.characters.width;
	unsigned int rows     = image.rows()    / options.characters.height;

	_LogInfo("Analyzing the image: " + std::to_string(columns) + "x" + std::to_string(rows));

	ImageData imageData(columns, rows);

	Magick::Image monoImage;
	if (options.image.useMono) {
		monoImage = Functions::makeMonochromatic(image);
	}
	for (unsigned int column = 0; column < columns; ++column) {
		for (unsigned int row = 0; row < rows; ++row) {
			LumaGrid      lumaGrid  = Functions::calculateLumaGrid(options.image.useMono ? monoImage : image, column, row);
			Magick::Color color     = Functions::calculateAverageColor(image, column, row);
			wchar_t       letter    = Functions::matchLetter(charactersData, lumaGrid);
			ImageCell     imageCell = ImageCell(lumaGrid, color, letter);
			imageData.setCell(column, row, imageCell);
		}
	}

	_LogInfo("Succesfully analyzed the image");

	return imageData;
}

Magick::Color Functions::calculateAverageColor(Magick::Image &image, unsigned int column, unsigned int row)
{
	Magick::Geometry geometry(image.columns(), image.rows());
	Magick::Quantum *pixels = image.getPixels(0, 0, geometry.width(), geometry.height());

	unsigned int channels = image.channels();
	unsigned long offset  = channels * (row * geometry.width() * options.characters.height + column * options.characters.width);

	Color averageColor = {0, 0, 0};
	for (unsigned int x = 0; x < options.characters.width; ++x) {
		for (unsigned int y = 0; y < options.characters.height; ++y) {
			unsigned long position = offset + channels * (y * geometry.width() + x);
			Color color = {pixels[position], pixels[position + 1], pixels[position + 2]};
			averageColor.red   += color.red;
			averageColor.green += color.green;
			averageColor.blue  += color.blue;
		}
	}

	averageColor.red   /= options.cellSize();
	averageColor.green /= options.cellSize();
	averageColor.blue  /= options.cellSize();

	unsigned long depth = 1 << image.depth();
	double lumaModifier = sqrt(Functions::getLuminosity(averageColor)) / depth;
	Magick::Color color = Magick::Color(averageColor.red / lumaModifier, averageColor.green / lumaModifier, averageColor.blue / lumaModifier);
	return color;
}

LumaGrid Functions::calculateLumaGrid(Magick::Image &image, unsigned int column, unsigned int row)
{
	Magick::Geometry geometry(image.columns(), image.rows());
	Magick::Quantum *pixels = image.getPixels(0, 0, geometry.width(), geometry.height());

	unsigned int channels = image.channels();
	unsigned long offset  = channels * (row * geometry.width() * options.characters.height + column * options.characters.width);

	LumaGrid lumaGrid;
	lumaGrid.resize(options.regions.columns, LumaVector(options.regions.rows, 0));
	for (unsigned int x = 0; x < options.characters.width; ++x) {
		for (unsigned int y = 0; y < options.characters.height; ++y) {
			unsigned int c = x / options.regions.columns;
			unsigned int r = y / options.regions.rows;
			unsigned long position = offset + channels * (y * geometry.width() + x);
			if (channels < 3) {
				lumaGrid[c][r] += pixels[position];
			} else {
				Color color = {pixels[position], pixels[position + 1], pixels[position + 2]};
				lumaGrid[c][r] += Functions::getLuminosity(color);
			}
		}
	}

	for (unsigned int c = 0; c < options.regions.columns; ++c) {
		for (unsigned int r = 0; r < options.regions.rows; ++r) {
			lumaGrid[c][r] /= options.regionSize() * DEPTH;
		}
	}

	return lumaGrid;
}

CharactersData Functions::calculateCharactersData()
{
	_LogInfo("Analyzing characters data...");
	CharactersData charactersData;
	for (size_t character = 0; character < options.characters.chars.size(); ++character) {
		wchar_t       letter        = options.characters.chars[character];
		Magick::Image letterImage   = Functions::getLetterImage(letter);
		LumaGrid      lumaGrid      = Functions::calculateLumaGrid(letterImage, 0, 0);
		CharacterData characterData = CharacterData(lumaGrid, "white", letter);
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

	if (std::find(CharactersList.begin(), CharactersList.end(), ' ') == CharactersList.end()) {
		CharactersList.push_back(' ');
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

Magick::Image Functions::drawOutput(const ImageData &imageData, std::string &outputString)
{
	_LogInfo("Creating an output image");
	Magick::Color backgroundColor = Magick::Color(options.image.backgroundColor);

	if (options.image.invertBackground) {
		Magick::Quantum red   = DEPTH - backgroundColor.quantumRed();
		Magick::Quantum green = DEPTH - backgroundColor.quantumGreen();
		Magick::Quantum blue  = DEPTH - backgroundColor.quantumBlue();
		backgroundColor = Magick::Color(red, green, blue);
	}

	Magick::Image newImage(Magick::Geometry(imageData.columns * options.characters.width, imageData.rows * options.characters.height), backgroundColor);
	newImage.strokeAntiAlias(options.characters.antialiasing);
	newImage.fontPointsize(options.characters.size);
	newImage.font("notomono.ttf");

	unsigned int yOffset = options.characters.size;
	for (unsigned int row = 0; row < imageData.rows; ++row) {
		for (unsigned int column = 0; column < imageData.columns; ++column) {
			std::string outputLetter;
			outputLetter += imageData[column][row].second;
			outputString += imageData[column][row].second;
			newImage.fillColor(imageData[column][row].first);
			newImage.draw(Magick::DrawableText(column * options.characters.width, row * options.characters.height + yOffset, outputLetter));
		}
		outputString += "\n";
	}

	_LogInfo("Succesfully created the output image");
	return newImage;
}

Magick::Image Functions::getLetterImage(wchar_t letter)
{
	Magick::Image letterImage(Magick::Geometry(options.characters.width, options.characters.height), "black");
	std::string letterString;
	letterString += letter;

	letterImage.type(Magick::GrayscaleType);
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

Frames Functions::makeASCII(Frames &frames)
{
	Frames outputFrames;
	for (size_t frame = 0; frame < frames.size(); ++frame) {
		_LogInfo("Image sequence: " + std::to_string(frame + 1) + "/" + std::to_string(frames.size()));

		std::string outputString;
		Magick::Image image = frames[frame];
		Magick::Image newImage = Functions::makeASCII(image, outputString);
		outputFrames.push_back(newImage);

		PRINT("Output:\n" << outputString);
	}

	return outputFrames;
}

Magick::Image Functions::makeASCII(Magick::Image &image, std::string &outputString)
{
	CharactersData charactersData = Functions::calculateCharactersData();
	ImageData      imageData      = Functions::analyzeImage(image, charactersData);
	Magick::Image  outputImage    = Functions::drawOutput(imageData, outputString);

	_LogInfo("Succesfully converted the image");
	return outputImage;
}

Magick::Image Functions::makeMonochromatic(Magick::Image &image, double threshold)
{
	_LogInfo("Creating a monochromatic image");
	Magick::Image monoImage(image);
	monoImage.type(Magick::GrayscaleType);

	Magick::Quantum *pixels = monoImage.getPixels(0, 0, monoImage.columns(), monoImage.rows());

	unsigned int channels = monoImage.channels();
	for (unsigned int x = 0; x < monoImage.columns(); ++x) {
		for (unsigned int y = 0; y < monoImage.rows(); ++y) {
			unsigned long position = channels * (x + monoImage.columns() * y);
			if (options.image.invertBackground) {
				pixels[position] = (pixels[position] <= threshold * DEPTH ? DEPTH : 0);
			} else {
				pixels[position] = (pixels[position]  > threshold * DEPTH ? DEPTH : 0);
			}
		}
	}

	_LogInfo("Successfully created a monochromatic image");

	return monoImage;
}

wchar_t Functions::matchLetter(const CharactersData &charactersData, const LumaGrid &lumaGrid)
{
	double minDistance = DBL_MAX;
	wchar_t letter = ' ';

	for (const auto &character : charactersData) {
		double distance = 0;
		CharacterData characterData = character.second;
		for (unsigned int row = 0; row < options.regions.rows; ++row) {
			for (unsigned int column = 0; column < options.regions.columns; ++column) {
				distance += square(lumaGrid[column][row] - characterData[column][row]);
			}
		}

		if (distance < minDistance) {
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

	if (variablesMap.count("threshold")) {
		options.image.threshold = variablesMap["threshold"].as<double>();
		options.image.useMono = true;
	}

	if (variablesMap.count("invert")) {
		options.image.invertBackground = true;
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

void Functions::readImages(Frames &frames, const std::string &inputPath)
{
	try {
		Magick::readImages(&frames, inputPath);
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

void Functions::writeImages(Frames &frames, const std::string &outputPath)
{
	try {
		Magick::writeImages(frames.begin(), frames.end(), outputPath);
		_LogInfo("Succesfully saved " + outputPath + " file");
	} catch (Magick::Exception &exception) {
		_LogError(exception.what());
	}
}
