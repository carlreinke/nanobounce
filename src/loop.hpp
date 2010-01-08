#ifndef LOOP_HPP
#define LOOP_HPP

#include "video.hpp"
#include "SDL.h"

class Loop
{
public:
	Loop( void ) { /* nothing to do */ };
	virtual ~Loop( void ) { /* nothing to do */ };
	
	void loop( SDL_Surface * );
	
protected:
	virtual void handle_event( SDL_Event & );
	
	virtual void update( void ) = 0;
	virtual void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const = 0;
	
	Fader fader;
	
	bool loop_quit;
};

#endif // LOOP_HPP
