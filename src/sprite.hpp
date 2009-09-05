#ifndef SPRITE_HPP
#define SPRITE_HPP

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
	
	unsigned int width( void ) const { return (surface != NULL) ? surface->w : w; }
	unsigned int height( void ) const { return (surface != NULL) ? surface->h : h; }
	
	void blit( SDL_Surface *, int x, int y, Uint8 alpha = SDL_ALPHA_OPAQUE) const;
	
protected:
	void load_ppm( std::istream &ppm );
	
	void copy( const Sprite & );
	void destroy( void );
	
	SDL_Surface *surface;
	
	SDL_Color color;
	unsigned int w, h;
};

#endif // SPRITE_HPP
