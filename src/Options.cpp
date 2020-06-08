#include "Options.hpp"

void Options::save(const std::string &filename)
{
	bpt::ptree pt;
	pt.put("screen.width",         SCREEN_WIDTH);
	pt.put("screen.height",        SCREEN_HEIGHT);
	pt.add("characters.size",      CHAR_SIZE);
	pt.add("characters.width",     CHAR_WIDTH);
	pt.add("characters.height",    CHAR_HEIGHT);
	pt.add("characters.color",     CHAR_COLOR);
	pt.add("characters.monospace", CHAR_MONOSPACE);
	pt.add("image.threshold",      IMAGE_THRESHOLD);
	write_ini(filename, pt);
}

void Options::load(const std::string &filename)
{
	bpt::ptree pt;
	read_ini(filename, pt);
	screen.width         = pt.get<unsigned short>("screen.width");
	screen.height        = pt.get<unsigned short>("screen.height");
	characters.size      = pt.get<unsigned short>("characters.size");
	characters.width     = pt.get<unsigned short>("characters.width");
	characters.height    = pt.get<unsigned short>("characters.height");
	characters.color     = pt.get<unsigned long>("characters.color");
	characters.monospace = pt.get<bool>("characters.monospace");
	image.threshold      = pt.get<double>("image.threshold");
}

