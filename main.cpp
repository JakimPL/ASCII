#include "main.hpp"

#include "src/Options.hpp"
#include "src/Functions.hpp"

int main(int argc, char *argv[])
{
	Functions::loadOptions();
	std::string inputPath, outputPath;
	int returnValue = Functions::parseProgramArguments(argc, argv, inputPath, outputPath);

	Magick::InitializeMagick(*argv);
	Magick::Image image;

	std::string outputString;
	Functions::readImage(image, inputPath);
	Magick::Image newImage = Functions::makeASCII(image, outputString);
	Functions::writeImage(newImage, outputPath);

	PRINT("Output:\n" << outputString);

	return returnValue;
}

