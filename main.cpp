#include <iostream>
#include <sstream>
#include <cmath>

#include <SFML/Graphics.hpp>

#include "src/Options.hpp"
#include "src/RichText.hpp"

#include "boost/program_options.hpp"
namespace bpo = boost::program_options;

#define PRINT(string) std::cout << string << std::endl
#define HELP "A simple program for generating ASCII art. Usage:\n./ASCII [input]\nExample:\n./ASCII image.png\nOptions"
#define VERSION "0.0.2"

Bitmap convertToMono(sf::Image &image, double threshold = options.image.threshold, sf::Color realColor = sf::Color(options.characters.color), bool setPixels = false)
{
	unsigned short imageWidth  = image.getSize().x;
	unsigned short imageHeight = image.getSize().y;

	Bitmap bitmap;
	bitmap.resize(imageWidth, std::vector<bool>(imageHeight, false));
	for (unsigned short x = 0; x < imageWidth; ++x) {
		for (unsigned short y = 0; y < imageHeight; ++y) {
			sf::Color color = image.getPixel(x, y);
			sf::Uint8 grey = 0.3f * color.r + 0.59f * color.g + 0.11 * color.b;
			bool white = (grey > 255 * threshold);
			bitmap[x][y] = white;
			sf::Color newColor = (white ? realColor : sf::Color::Black);

			if (setPixels) {
				image.setPixel(x, y, newColor);
			}
		}
	}

	return bitmap;
}

Charmaps createCharmaps(const Chars &chars, sf::Font &font)
{
	Charmaps charmaps;
	for (size_t character = 0; character < chars.size(); ++character) {
		sf::Glyph glyph = font.getGlyph(chars[character], options.characters.size, false);
		sf::Texture map = font.getTexture(options.characters.size);

		unsigned short xOffset = options.characters.width - glyph.bounds.width;
		unsigned short yOffset = options.characters.height - glyph.bounds.height;

		sf::Image glyphImage;

		glyphImage.create(options.characters.width, options.characters.height);
		glyphImage.copy(map.copyToImage(), xOffset / 2, yOffset / 2, glyph.textureRect);

		Bitmap glyphBitmap = convertToMono(glyphImage);
		Charmap charmap = std::make_pair(chars[character], glyphBitmap);
		charmaps.push_back(charmap);
	}

	return charmaps;
}

unsigned long calculateDistance(const Bitmap &bitmap1, const Bitmap &bitmap2)
{
	unsigned long distance = 0;
	for (size_t x = 0; x < bitmap1.size(); ++x) {
		for (size_t y = 0; y < bitmap1[x].size(); ++y) {
			distance += (bitmap1[x][y] != bitmap2[x][y]);
		}
	}

	return distance;
}

wchar_t matchLetter(const Charmaps charmaps, Bitmap &fragment)
{
	size_t size = charmaps.size();
	size_t id = 0;
	unsigned long minDistance = charmaps[0].second.size() * (charmaps[0].second)[0].size();
	for (size_t character = 0; character < size; ++character) {
		unsigned long distance = calculateDistance(charmaps[character].second, fragment);
		if (distance <= minDistance) {
			id = character;
			minDistance = distance;
		}
	}

	return charmaps[id].first;
}

sf::Color getAverageColor(sf::Image &image)
{
	unsigned short width  = image.getSize().x;
	unsigned short height = image.getSize().y;
	unsigned long size = width * height;

	unsigned long r = 0;
	unsigned long g = 0;
	unsigned long b = 0;

	for (unsigned short y = 0; y < height; ++y) {
		for (unsigned short x = 0; x < width; ++x) {
			sf::Color color = image.getPixel(x, y);
			r += color.r;
			g += color.g;
			b += color.b;
		}
	}

	return sf::Color(r / size, g / size, b / size);
}

int main(int argc, char *argv[])
{
	bpo::options_description description(HELP);
	description.add_options()
	("help,h",                                   "Shows the help message")
	("version,v",                                "Shows the version of the program")
	("input,i",       bpo::value<std::string>(), "A path to the input image file");

	bpo::variables_map variablesMap;
	bpo::positional_options_description positionalOptionsDescription;
	positionalOptionsDescription.add("input", 1);

	try {
		bpo::store(bpo::command_line_parser(argc, argv).options(description).positional(positionalOptionsDescription).run(), variablesMap);
		bpo::notify(variablesMap);
	} catch (std::exception &exception) {
		PRINT("Command option error: " << exception.what());
		return -2;
	} catch (...) {
		PRINT("Unknown error");
		return -2;
	}

	bool exit = false;
	if (variablesMap.count("help")) {
		PRINT(description);
		exit = true;
	}

	if (variablesMap.count("version")) {
		PRINT("Version: " << VERSION);
		exit = true;
	}

	std::string inputPath;
	if (variablesMap.count("input")) {
		inputPath = variablesMap["input"].as<std::string>();
		std::fstream file(inputPath);
		if (!file.good()) {
			PRINT("Failed to load the input file!");
			return -1;
		}
	} else {
		PRINT(description << std::endl << "Version: " << VERSION);
		return 1;
	}


	if (exit) {
		return 1;
	}

	//options.save("options.ini");
	options.load("options.ini");

	Bitmap bitmap;
	sf::Event event;
	sf::Image image;
	sf::Texture texture;
	sf::Sprite sprite;
	sf::Font font;

	font.loadFromFile("notomono.ttf");
	image.loadFromFile(inputPath);
	bitmap = convertToMono(image);
	texture.loadFromImage(image);
	sprite.setTexture(texture);

	options.screen.width  = image.getSize().x;
	options.screen.height = image.getSize().y;

	sf::RenderWindow window(sf::VideoMode(options.screen.width, options.screen.height), "Dots");
	Charmaps charmaps = createCharmaps(options.characters.chars, font);

	sfe::RichText text(font);
	text.setCharacterSize(options.characters.size);
	unsigned short width  = options.screen.width  / options.characters.width;
	unsigned short height = options.screen.height / options.characters.height;
	for (unsigned short y = 0; y < height; ++y) {
		for (unsigned short x = 0; x < width; ++x) {
			sf::Image fragment;
			Bitmap fragmentBitmap;
			fragment.create(options.characters.width, options.characters.height);
			fragment.copy(image, 0, 0, sf::IntRect(x * options.characters.width, y * options.characters.height, (x + 1) * options.characters.width, (y + 1) * options.characters.height));
			fragmentBitmap = convertToMono(fragment);
			text << getAverageColor(fragment) << matchLetter(charmaps, fragmentBitmap);
		}
		text << "\n";
	}

	unsigned long time = 0;
	while (window.isOpen()) {
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed or sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
				window.close();
				break;
			}
		}

		window.clear();
		//window.draw(sprite);
		window.draw(text);
		window.display();
		time++;
	}

	return 0;
}

