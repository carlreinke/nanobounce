#ifndef LOOP_HPP
#define LOOP_HPP

#include "fader.hpp"
#include "misc.hpp"

#include <SDL.h>

static const uint update_per_sec = 125, msec_per_update = 1000 / update_per_sec;

static inline uint ms_to_updates( uint ms ) { return (ms / msec_per_update); }

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
	
	SDL_Surface *surface;
	Fader fader;
	
	bool loop_quit;
};

#endif // LOOP_HPP
