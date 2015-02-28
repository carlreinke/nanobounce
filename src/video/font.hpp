/*  video/font.hpp
 *  
 *  Copyright 2015 Carl Reinke
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
#ifndef VIDEO_FONT_HPP
#define VIDEO_FONT_HPP

#include "video/sprite.hpp"

class Font
{
public:
	Font( void );
	
	void load( const std::string &pgm_path, const std::string &meta_path );
	void load_pgm( std::istream &pgm );
	void load_char_widths( std::istream & );
	
	enum Justification
	{
		LEFT,
		CENTER,
		RIGHT
	};
	
	enum Style
	{
		NORMAL,
		MAJUSCULE,
		MINUSCULE
	};
	
	void blit( SDL_Surface *, int x, int y, const std::string &, const Sprite &, Justification = LEFT, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	void blit( SDL_Surface *, int x, int y, std::string, const Sprite &, Style, Justification = LEFT, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
	uint height( const Sprite &sprite ) const { return graymap.size() * sprite.height(); };
	uint width( const std::string &, const Sprite & ) const;
	
private:
	void blit_char( SDL_Surface *, int x, int y, char, const Sprite &, Uint8 alpha ) const;
	
	typedef std::vector< std::vector<unsigned char> > Graymap;
	
	Graymap graymap;
	std::vector<uint> char_positions;
	std::vector<uint> char_widths;
};

extern Font font;
extern std::vector<Sprite> font_sprites;

#endif // VIDEO_FONT_HPP
