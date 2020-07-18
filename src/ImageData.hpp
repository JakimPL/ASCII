#ifndef IMAGEDATA_HPP
#define IMAGEDATA_HPP

#include <vector>

#include <Magick++/lib/Magick++.h>
#include <MagickWand/MagickWand.h>
#include <MagickCore/MagickCore.h>

#include "ImageCell.hpp"

class ImageData
{
public:
	ASCIIData asciiData;
	const unsigned int &columns = mColumns;
	const unsigned int &rows = mRows;
private:
	unsigned int mColumns;
	unsigned int mRows;

public:
	ImageData(unsigned int c, unsigned int r);

	void setCell(unsigned int column, unsigned int row, ImageCell imageCell);
	void setCell(unsigned int column, unsigned int row, Magick::Color color, wchar_t letter);

	const ASCIIVector& operator[](unsigned row) const
	{
		return asciiData[row];
	}
};

#endif
