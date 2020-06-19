#ifndef MAIN_HPP
#define MAIN_HPP

#include <iostream>
#include <sstream>
#include <cmath>

#include <Magick++/lib/Magick++.h>
#include <MagickWand/MagickWand.h>
#include <MagickCore/MagickCore.h>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

namespace bfs = boost::filesystem;
namespace bpo = boost::program_options;
namespace bpt = boost::property_tree;

typedef std::vector<wchar_t> CharacterList;
typedef std::vector<double> Luma;
typedef std::vector<Luma> CharactersLuminosity;

#define PRINT(string) std::cout << string << std::endl
#define FULL_IMAGE(image) 0, 0, image.columns(), image.rows()
#define HELP "A simple program for generating ASCII art. Usage:\n./ASCII [input] [output]\nExample:\n./ASCII image.png image_output.png\nOptions"
#define VERSION "0.1.0"
#define OPTIONS_INI "options.ini"

#endif // MAIN_HPP
