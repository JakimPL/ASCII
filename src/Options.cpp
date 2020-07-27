#include "Options.hpp"
#include "Functions.hpp"

void Options::save(const std::string &filename)
{
	bpt::ptree pt;
	pt.add("colorsWeights.red",        colorsWeights.red);
	pt.add("colorsWeights.green",      colorsWeights.green);
	pt.add("colorsWeights.blue",       colorsWeights.blue);
	pt.add("characters.chars",         Functions::convertCharsToString(characters.chars));
	pt.add("characters.size",          characters.size);
	pt.add("characters.width",         characters.width);
	pt.add("characters.height",        characters.height);
	pt.add("characters.antialiasing",  characters.antialiasing);
	pt.add("image.backgroundColor",    image.backgroundColor);
	pt.add("image.invertBackground",   image.invertBackground);
	pt.add("image.useMono",            image.useMono);
	pt.add("image.threshold",          image.threshold);
	pt.add("regions.columns",          regions.columns);
	pt.add("regions.rows",             regions.rows);
	write_ini(filename, pt);
}

void Options::load(const std::string &filename)
{
	bpt::ptree pt;
	read_ini(filename, pt);
	colorsWeights.red        = pt.get<float>("colorsWeights.red");
	colorsWeights.green      = pt.get<float>("colorsWeights.green");
	colorsWeights.blue       = pt.get<float>("colorsWeights.blue");
	characters.chars         = Functions::convertStringToChars(pt.get<std::string>("characters.chars"));
	characters.size          = pt.get<unsigned short>("characters.size");
	characters.width         = pt.get<unsigned short>("characters.width");
	characters.height        = pt.get<unsigned short>("characters.height");
	characters.antialiasing  = pt.get<bool>("characters.antialiasing");
	image.backgroundColor    = pt.get<std::string>("image.backgroundColor");
	image.invertBackground   = pt.get<bool>("image.invertBackground");
	image.useMono            = pt.get<bool>("image.useMono");
	image.threshold          = pt.get<float>("image.threshold");
	regions.columns          = pt.get<unsigned int>("regions.columns");
	regions.rows             = pt.get<unsigned int>("regions.rows");
}
