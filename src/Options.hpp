#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include "Types.hpp"

#define COLORS_WEIGHTS_RED      0.30f
#define COLORS_WEIGHTS_GREEN    0.59f
#define COLORS_WEIGHTS_BLUE     0.11f

#define CHARS                   {' ', '.', ':', ';', 'o', 'x', '%', '#', '@'}
#define CHAR_SIZE               13
#define CHAR_WIDTH              8
#define CHAR_HEIGHT             15
#define CHAR_ANTIALIASING       false

#define IMAGE_BACKGROUND_COLOR "#000000"
#define IMAGE_USE_MONO          false
#define IMAGE_THRESHOLD         0.5f

#define REGION_COLUMNS          3
#define REGION_ROWS             4

#define DEPTH                   65536

struct Options {
	Color  colorsWeights               = {COLORS_WEIGHTS_RED, COLORS_WEIGHTS_GREEN, COLORS_WEIGHTS_BLUE};
	struct Characters {
		CharactersList chars           = CHARS;
		unsigned short size            = CHAR_SIZE;
		unsigned short width           = CHAR_WIDTH;
		unsigned short height          = CHAR_HEIGHT;
		bool           antialiasing    = CHAR_ANTIALIASING;
	} characters;
	struct Image {
		std::string    backgroundColor = IMAGE_BACKGROUND_COLOR;
		bool           useMono         = IMAGE_USE_MONO;
		double         threshold       = IMAGE_THRESHOLD;
	} image;
	struct Regions {
		unsigned long  columns         = REGION_COLUMNS;
		unsigned long  rows            = REGION_ROWS;
	} regions;

	void save(const std::string &filename);
	void load(const std::string &filename);

	constexpr unsigned int cellSize()
	{
		return characters.width * characters.height;
	}
	constexpr unsigned int regionSize()
	{
		return cellSize() / (regions.columns * regions.rows);
	}
};

static Options options;

#endif
