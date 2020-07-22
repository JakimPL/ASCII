#include "main.hpp"
#include "src/Functions.hpp"

int main(int argc, char *argv[])
{
	Functions::loadOptions();
	std::string inputPath, outputPath;
	int returnValue = Functions::parseProgramArguments(argc, argv, inputPath, outputPath);

	Magick::InitializeMagick(*argv);
	Frames frames;
	Functions::readImages(frames, inputPath);
	Frames outputFrames = Functions::makeASCII(frames);
	Functions::writeImages(outputFrames, outputPath);

	return returnValue;
}

