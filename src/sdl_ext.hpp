#ifndef SDL_EXT_HPP
#define SDL_EXT_HPP

#include "SDL.h"

class SDL_Surface_smartptr
{
public:
	SDL_Surface_smartptr( int w = 0, int h = 0, int bpp = 0 ) : surface(NULL) { resize(w, h, bpp); }
	~SDL_Surface_smartptr( void ) { SDL_FreeSurface(surface); }
	
	operator SDL_Surface *( void ) { return surface; }
	
	void resize( int w, int h, int bpp )
	{
		SDL_FreeSurface(surface);
		surface = SDL_CreateRGBSurface(SDL_HWSURFACE || SDL_HWACCEL, w, h, bpp, 0, 0, 0, 0);
	}
	
private:
	SDL_Surface *surface;
};

inline int SDL_FillRect( SDL_Surface *dst, SDL_Rect *dstrect, const SDL_Color &color )
{
	return SDL_FillRect(dst, dstrect, SDL_MapRGB(dst->format, color.r, color.g, color.b));
}

inline int SDL_FillRectA( SDL_Surface *dst, SDL_Rect *dstrect, const SDL_Color &color )
{
	return SDL_FillRect(dst, dstrect, SDL_MapRGBA(dst->format, color.r, color.g, color.b, color.unused));
}

inline SDL_Color SDL_Color_RGBA( int r, int g, int b, int a = 0 )
{
	SDL_Color c = { r, g, b, a };
	return c;
}

#endif // SDL_EXT_HPP
