#ifndef SPRITE_H
#define SPRITE_H

#include "SDL.h"

class Sprite
{
public:
	Sprite( void );
	Sprite( unsigned int width, unsigned int height, const SDL_Color &color );
	
	Sprite( const std::string &netpbm_file );
	Sprite( std::istream &netpbm );
	
	Sprite( const Sprite & );
	~Sprite( void );
	
	Sprite & operator=( const Sprite & );
	
	unsigned int width( void ) const { return (surface != NULL) ? surface->w : 0; }
	unsigned int height( void ) const { return (surface != NULL) ? surface->h : 0; }
	
	void blit( SDL_Surface *, int x, int y, Uint8 alpha = SDL_ALPHA_OPAQUE) const;
	
protected:
	void load_ppm( std::istream &ppm );
	
	void copy( const Sprite & );
	void destroy( void );
	
	SDL_Surface *surface;
};

#endif
