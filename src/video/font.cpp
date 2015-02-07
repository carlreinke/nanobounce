/*  video/font.cpp
 *  
 *  Copyright 2010 Carl Reinke
 *  
 *  This program is non-commercial, open-source software; you can redistribute
 *  it and/or modify it under the terms of the MAME License as included along
 *  with this program.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  MAME License for more details.
 *  
 *  You should have received a copy of the MAME License along with this
 *  program; if not, see <http://www.intoorbit.org/licenses/MAME.txt>.
 */
#include "misc.hpp"
#include "video/font.hpp"

Font font;
std::vector<Sprite> font_sprites;

Font::Font( void )
: char_positions(256), char_widths(256)
{
	// nothing to do
}

void Font::load( const std::string &pgm_path, const std::string &meta_path )
{
	std::ifstream pgm(pgm_path.c_str());
	load_pgm(pgm);
	
	std::ifstream meta(meta_path.c_str());
	load_char_widths(meta);
}

void Font::load_pgm( std::istream &pgm )
{
	std::string type;
	pgm >> type;
	if (type != "P2")
	{
		std::cerr << "error: font loading failed" << std::endl;
		return;
	}
	
	uint w = get_no_comments<uint>(pgm),
	     h = get_no_comments<uint>(pgm),
	     component_max = get_no_comments<uint>(pgm);
	
	graymap.resize(h);
	
	uint x = 0, y = 0;
	
	while (pgm.good() && y < h)
	{
		int g = get_no_comments<uint>(pgm) * component_max / 255;
		graymap[y].push_back(g);
		
		if (++x == w)
		{
			x = 0;
			++y;
		}
	}
}

void Font::load_char_widths( std::istream &is )
{
	if (graymap.size() == 0)
		return;
	
	uint x = 0;
	
	while (is.good() && x < graymap[0].size())
	{
		char c = (x == 0) ? ' ' : get_no_comments<char>(is);
		int w = get_no_comments<int>(is);
		
		char_positions[c] = x;
		char_widths[c] = w;
		
		x += w;
	}
	
	assert(x == graymap[0].size());
}

void Font::blit( SDL_Surface *surface, int x, int y, const std::string &text, const Sprite &sprite, Justification justify, Uint8 alpha ) const
{
	switch (justify)
	{
	case CENTER:
		x -= (width(text, sprite)) / 2;
		break;
	case RIGHT:
		x -= width(text, sprite);
		break;
	case LEFT:
		break;
	}
	
	const int start_x = x;
	
	for (const auto scanline : graymap)
	{
		for (const unsigned char c : text)
		{
			uint char_position = char_positions[c];
			
			for (uint char_x = 0; char_x < char_widths[c]; ++char_x)
			{
				if (scanline[char_position + char_x] > 0)
					sprite.blit(surface, x, y, alpha);
				
				x += sprite.width();
			}
		}
		
		y += sprite.width();
		x = start_x;
	}
}

void Font::blit( SDL_Surface *surface, int x, int y, std::string text, const Sprite &sprite, Style style, Justification justify, Uint8 alpha ) const
{
	switch (style)
	{
	case MAJUSCULE:
		for (char &c : text)
			c = toupper(static_cast<unsigned char>(c));
		break;
	case MINUSCULE:
		for (char &c : text)
			c = tolower(static_cast<unsigned char>(c));
		break;
	case NORMAL:
	default:
		break;
	}
	
	blit(surface, x, y, text, sprite, justify, alpha);
}

uint Font::width( const std::string &text, const Sprite &sprite ) const
{
	uint width = 0;
	
	for (const unsigned char c : text)
		width += char_widths[c] * sprite.width();
	
	return width;
}
