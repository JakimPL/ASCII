#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include "ImageData.hpp"
#include "Options.hpp"

typedef std::vector<ImageData> CharactersData;

namespace Functions
{

bpo::options_description addProgramDescription();
CharactersData calculateCharactersData();
LumaGrid calculateLuma(const Magick::Image &image);
LumaGrid calculateLuma(const Magick::Image &image, unsigned long xOffset, unsigned long yOffset);
std::string convertCharsToString(const CharacterList &chars);
CharacterList convertStringToChars(const std::string &chars);
Magick::Image getASCII(const Magick::Image &image);
Magick::Color getAverageColor(const Magick::Image &image);
Magick::Color getAverageColor(const Magick::Image &image, unsigned int xstart, unsigned int ystart, unsigned int width, unsigned int height);
Magick::Image getLetterImage(wchar_t letter);
double getLuminosity(const Magick::Color &color);
bpo::variables_map getVariablesMap(bpo::options_description description, int argc, char *argv[]);
void loadOptions();
wchar_t matchLetter(const CharactersData &charactersData, const LumaGrid &luma);
int parseProgramArguments(int argc, char *argv[], std::string &inputPath, std::string &outputPath);
void readImage(Magick::Image &image, const std::string &inputPath);

template <typename Type>
inline double square(Type x)
{
	return x * x;
}

/*template <typename Type>
Type sum(const std::vector<Type> &vector);

template <typename Type>
Type sum(const std::vector<std::vector<Type>> &grid);
*/

}

#endif
