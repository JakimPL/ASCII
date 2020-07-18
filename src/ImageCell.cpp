#include "ImageCell.hpp"
#include "Options.hpp"
#include "main.hpp"

ImageCell::ImageCell(LumaGrid lG, Magick::Color col, wchar_t l)
{
	lumaGrid = lG;
	color = col;
	letter = l;
}

ImageCell::ImageCell()
{

}
