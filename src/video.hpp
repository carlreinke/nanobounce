#ifndef VIDEO_HPP
#define VIDEO_HPP

#include "SDL.h"

extern int screen_width, screen_height, screen_bpp;

SDL_Surface *init_video( void );

class Fader
{
public:
	Fader( int step = 15 );
	
	enum Direction
	{
		in = SDL_ALPHA_OPAQUE,
		out = SDL_ALPHA_TRANSPARENT
	};
	
	void fade( Direction );
	
	void update( void );
	int value( void ) const { return alpha; }
	
	bool was_fading( Direction direction ) const { return (target == (int)direction); }
	bool is_fading( Direction direction ) const { return (!is_done() && target == (int)direction); }
	bool is_done( void ) const { return done; }
	
private:
	bool done;
	int alpha;
	int target, step;
};

#endif // VIDEO_HPP
