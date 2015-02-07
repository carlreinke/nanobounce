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
		in = SDL_ALPHA_OPAQUE,
		out = SDL_ALPHA_TRANSPARENT
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
