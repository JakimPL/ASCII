#include "main.hpp"

#include "src/Options.hpp"
#include "src/Functions.hpp"

int main(int argc, char *argv[])
{
	Functions::loadOptions();
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

	std::string inputPath, outputPath;
	if (variablesMap.count("input")) {
		inputPath = variablesMap["input"].as<std::string>();
		std::fstream file(inputPath);
		if (!file.good()) {
			PRINT("Failed to load the input file!");
			return -1;
		}

		if (variablesMap.count("output")) {
			outputPath = variablesMap["output"].as<std::string>();
			if (outputPath == inputPath) {
				PRINT("Output path cannot be the same as input path!");
				return -1;
			}
		} else {
			PRINT("No output path specified!");
			return -1;
		}
	} else {
		PRINT(description << std::endl << "Version: " << VERSION);
		return 1;
	}

	if (exit) {
		return 1;
	}

	Magick::InitializeMagick(*argv);
	Magick::Image image;

	Functions::readImage(image, inputPath);
	Magick::Image newImage = Functions::getASCII(image);
	newImage.write(outputPath);

	return 0;
}

