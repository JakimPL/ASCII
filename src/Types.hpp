#ifndef TYPES_H
#define TYPES_H

#include <vector>

#include <Magick++/lib/Magick++.h>
#include <MagickWand/MagickWand.h>
#include <MagickCore/MagickCore.h>

struct Color {
	float red   = 0;
	float green = 0;
	float blue  = 0;
};

typedef std::vector<wchar_t> CharactersList;
typedef std::vector<double> LumaVector;
typedef std::vector<LumaVector> LumaGrid;
typedef std::vector<Magick::Color> ColorVector;
typedef std::vector<ColorVector> ColorGrid;
typedef std::vector<std::pair<Magick::Color, wchar_t>> ASCIIVector;
typedef std::vector<ASCIIVector> ASCIIData;
typedef std::vector<Magick::Image> Frames;

#endif
