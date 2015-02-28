/*  nanobounce/src/fader.hpp
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
#ifndef FADER_HPP
#define FADER_HPP

#include "misc.hpp"

#include <SDL.h>

class Fader
{
public:
	Fader( uint ms = 0 );
	
	enum Direction
	{
		IN = SDL_ALPHA_OPAQUE,
		OUT = SDL_ALPHA_TRANSPARENT
	};
	
	void fade( Direction );
	
	void update( void );
	int value( void ) const { return alpha; }
	
	bool was_fading( Direction direction ) const { return (static_cast<Direction>(target) == direction); }
	bool is_fading( Direction direction ) const { return (!is_done() && static_cast<Direction>(target) == direction); }
	bool is_done( void ) const { return done; }
	
private:
	bool done;
	int alpha;
	int target, step;
};

#endif // FADER_HPP
