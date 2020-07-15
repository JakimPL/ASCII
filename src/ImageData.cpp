#include "ImageData.hpp"
#include "Functions.hpp"

ImageData::ImageData(unsigned c, unsigned r)
{
	mColumns = c;
	mRows = r;

	totalLuma = 0;
	lumaGrid.resize(mRows);
	colorGrid.resize(mRows);

	for (size_t row = 0; row < mRows; ++row) {
		lumaGrid[row].resize(mColumns);
		colorGrid[row].resize(mColumns);
	}
}

void ImageData::calculateLuma(const Magick::Image &letterImage)
{
	lumaGrid = Functions::calculateLuma(letterImage);
}

double ImageData::getLuma(unsigned int row, unsigned int column)
{
	return lumaGrid[row][column];
}

wchar_t ImageData::getLetter()
{
	return letter;
}

void ImageData::setLetter(wchar_t l)
{
	letter = l;
}
