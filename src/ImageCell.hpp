#ifndef IMAGECELL_HPP
#define IMAGECELL_HPP

#include "Types.hpp"

class ImageCell
{
public:
	ImageCell(LumaGrid lG, Magick::Color col, wchar_t l = ' ');
	ImageCell();
private:
	wchar_t letter;
	double totalLuma;
	Magick::Color color;
	LumaGrid lumaGrid;
public:
	wchar_t getLetter()
	{
		return letter;
	};
	double getLuma(unsigned int column, unsigned row)
	{
		return lumaGrid[column][row];
	};
	double getTotalLuma()
	{
		return totalLuma;
	};
	Magick::Color getColor()
	{
		return color;
	};
	LumaGrid getLumaGrid()
	{
		return lumaGrid;
	};
	void setLetter(wchar_t l)
	{
		letter = l;
	};
	const LumaVector& operator[](unsigned column) const
	{
		return lumaGrid[column];
	}
};

typedef std::vector<ImageCell> ImageCellVector;
typedef std::vector<ImageCellVector> ImageCellGrid;
typedef std::map<wchar_t, ImageCell> CharactersData;
using CharacterData = ImageCell;

#endif
