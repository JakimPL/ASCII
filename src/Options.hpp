#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include "main.hpp"

#define COLORS_WEIGHTS_RED      0.30f
#define COLORS_WEIGHTS_GREEN    0.59f
#define COLORS_WEIGHTS_BLUE     0.11f

#define CHARS                   {' ', '.', ':', ';', 'o', 'x', '%', '#', '@'}
#define CHAR_SIZE               13
#define CHAR_WIDTH              8
#define CHAR_HEIGHT             15
#define CHAR_ANTIALIASING       false

#define IMAGE_BACKGROUND_COLOR "#000000"

#define REGION_COLUMNS          3
#define REGION_ROWS             3

struct Color {
	float red   = 0;
	float green = 0;
	float blue  = 0;
};

struct Options {
	Color  colorsWeights               = {COLORS_WEIGHTS_RED, COLORS_WEIGHTS_GREEN, COLORS_WEIGHTS_BLUE};
	struct Characters {
		CharacterList  chars           = CHARS;
		unsigned short size            = CHAR_SIZE;
		unsigned short width           = CHAR_WIDTH;
		unsigned short height          = CHAR_HEIGHT;
		bool           antialiasing    = CHAR_ANTIALIASING;
	} characters;
	struct Image {
		std::string    backgroundColor = IMAGE_BACKGROUND_COLOR;
	} image;
	struct Regions {
		unsigned long  columns         = REGION_COLUMNS;
		unsigned long  rows            = REGION_ROWS;
	} regions;

	void save(const std::string &filename);
	void load(const std::string &filename);
};

static Options options;

#endif
