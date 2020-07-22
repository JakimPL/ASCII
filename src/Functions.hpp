#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include "main.hpp"
#include "Options.hpp"
#include "ImageData.hpp"

namespace Functions
{

bpo::options_description addProgramDescription();
ImageData analyzeImage(Magick::Image &image, const CharactersData &charactersData);
Magick::Color calculateAverageColor(Magick::Image &image, unsigned int column, unsigned int row);
LumaGrid calculateLumaGrid(Magick::Image &image, unsigned int column, unsigned int row);
CharactersData calculateCharactersData();
std::string convertCharsToString(const CharactersList &chars);
CharactersList convertStringToChars(const std::string &chars);
Magick::Image drawOutput(const ImageData &imageData, std::string &outputString);
Magick::Image getLetterImage(wchar_t letter);
bpo::variables_map getVariablesMap(bpo::options_description description, int argc, char *argv[]);
void loadOptions();
Frames makeASCII(Frames &frames);
Magick::Image makeASCII(Magick::Image &image, std::string &outputString);
Magick::Image makeMonochromatic(Magick::Image &image, double threshold = options.image.threshold);
wchar_t matchLetter(const CharactersData &charactersData, const LumaGrid &lumaGrid);
int parseProgramArguments(int argc, char *argv[], std::string &inputPath, std::string &outputPath);
void readImage(Magick::Image &image, const std::string &inputPath);
void readImages(Frames &frames, const std::string &inputPath);
void writeImage(Magick::Image &image, const std::string &outputPath);
void writeImages(Frames &frames, const std::string &outputPath);

inline double getLuminosity(double red, double green, double blue)
{
	return options.colorsWeights.red   * red   +
		   options.colorsWeights.green * green +
		   options.colorsWeights.blue  * blue;
}
inline double getLuminosity(const Color &color)
{
	return options.colorsWeights.red   * color.red   +
		   options.colorsWeights.green * color.green +
		   options.colorsWeights.blue  * color.blue;
}

inline double getLuminosity(const Magick::Color &color)
{
	return options.colorsWeights.red   * color.quantumRed()   +
		   options.colorsWeights.green * color.quantumGreen() +
		   options.colorsWeights.blue  * color.quantumBlue();
}

template <typename Type>
inline double square(Type x)
{
	return x * x;
}

}

#endif
