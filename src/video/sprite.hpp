/*  video/sprite.hpp
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
#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "misc.hpp"
#include "SDL.h"

class Sprite
{
public:
	Sprite( void );
	Sprite( uint width, uint height, const SDL_Color &color );
	
	Sprite( const std::string &netpbm_file );
	Sprite( std::istream &netpbm );
	
	Sprite( const Sprite &, const SDL_Rect & );
	
	Sprite( const Sprite & );
	~Sprite( void );
	
	Sprite & operator=( const Sprite & );
	
	uint width( void ) const { return (surface != NULL) ? surface->w : w; }
	uint height( void ) const { return (surface != NULL) ? surface->h : h; }
	
	void blit( SDL_Surface *, Sint16 x, Sint16 y, Uint8 alpha = SDL_ALPHA_OPAQUE) const;
	
protected:
	void load_ppm( std::istream &ppm );
	
	void copy( const Sprite & );
	void destroy( void );
	
	SDL_Surface *surface;
	
	SDL_Color color;
	uint w, h;
};

#endif // SPRITE_HPP
