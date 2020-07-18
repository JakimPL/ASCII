#include "ImageData.hpp"

ImageData::ImageData(unsigned int c, unsigned int r)
{
	mColumns = c;
	mRows = r;

	asciiData.resize(mColumns);
	for (size_t column = 0; column < mColumns; ++column) {
		asciiData[column].resize(mRows);
	}
}

void ImageData::setCell(unsigned int column, unsigned int row, ImageCell imageCell)
{
	asciiData[column][row].first  = imageCell.getColor();
	asciiData[column][row].second = imageCell.getLetter();
}

void ImageData::setCell(unsigned int column, unsigned int row, Magick::Color color, wchar_t letter)
{
	asciiData[column][row].first  = color;
	asciiData[column][row].second = letter;
}
