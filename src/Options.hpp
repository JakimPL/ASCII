#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <SFML/Graphics.hpp>

namespace bpt = boost::property_tree;
typedef std::vector<std::vector<bool>> Bitmap;
typedef std::vector<char> Chars;
typedef std::pair<char, Bitmap> Charmap;
typedef std::vector<Charmap> Charmaps;

#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600

#define CHAR_SIZE       13
#define CHAR_WIDTH      8
#define CHAR_HEIGHT     15
#define CHAR_COLOR      0xddbbcc
#define CHAR_MONOSPACE  true

#define IMAGE_THRESHOLD 0.5f

struct Options {
	struct Screen {
		unsigned short width  = SCREEN_WIDTH;
		unsigned short height = SCREEN_HEIGHT;
	} screen;
	struct Characters {
		unsigned short size   = CHAR_SIZE;
		unsigned short width  = CHAR_WIDTH;
		unsigned short height = CHAR_HEIGHT;
		unsigned long  color  = CHAR_COLOR;
		bool monospace        = CHAR_MONOSPACE;
	} characters;
	struct Image {
		double threshold      = IMAGE_THRESHOLD;
	} image;

	void save(const std::string &filename);
	void load(const std::string &filename);
};

static Options options;
static const Chars chars = {' ', '.', 'X', '&', '|', '+', '$', '[', '=', 'o', '#', '!', '<'};

#endif
