#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include "Options.hpp"

namespace Functions
{

bpo::options_description addProgramDescription();
CharactersLuminosity calculateCharactersLuminosity();
Luma calculateLuma(const Magick::Image &image);
Luma calculateLuma(const Magick::Image &image, unsigned long xOffset, unsigned long yOffset);
std::string convertCharsToString(const CharacterList &chars);
CharacterList convertStringToChars(const std::string &chars);
Magick::Image getASCII(const Magick::Image &image);
Magick::Color getAverageColor(const Magick::Image &image);
Magick::Color getAverageColor(const Magick::Image &image, unsigned int xstart, unsigned int ystart, unsigned int width, unsigned int height);
Magick::Image getLetterImage(std::string);
double getLuminosity(const Magick::Color &color);
bpo::variables_map getVariablesMap(bpo::options_description description, int argc, char *argv[]);
void loadOptions();
wchar_t matchLetter(CharactersLuminosity charactersLuminosity, const Luma &luma);
void readImage(Magick::Image &image, const std::string &inputPath);

}

#endif // FUNCTIONS_HPP
