#ifndef IMAGEDATA_HPP
#define IMAGEDATA_HPP

#include <vector>

#include <Magick++/lib/Magick++.h>
#include <MagickWand/MagickWand.h>
#include <MagickCore/MagickCore.h>

#include "main.hpp"

class ImageData
{
public:
	ImageData(unsigned c, unsigned r);

public:
	const unsigned int &columns = mColumns;
	const unsigned int &rows = mRows;
private:
	wchar_t letter;

	unsigned int mColumns;
	unsigned int mRows;

	double totalLuma;
	LumaGrid lumaGrid;
	ColorGrid colorGrid;
public:
	void calculateLuma(const Magick::Image &letterImage);
	wchar_t getLetter();
	void setLetter(wchar_t l);

	double getLuma(unsigned int row, unsigned int column);
};

#endif
