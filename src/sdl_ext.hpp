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

inline SDL_Surface *SDL_DuplicateRGBSurface( SDL_Surface *src )
{
	SDL_Surface *dst = SDL_CreateRGBSurface(SDL_HWSURFACE, src->w, src->h, src->format->BitsPerPixel, src->format->Rmask, src->format->Gmask, src->format->Bmask, src->format->Amask);
	if (dst != NULL)
		SDL_BlitSurface(src, NULL, dst, NULL);
	return dst;
}

// rectangle

inline int SDL_FillRect( SDL_Surface *dst, SDL_Rect *dstrect, const SDL_Color &color )
{
	return SDL_FillRect(dst, dstrect, SDL_MapRGB(dst->format, color.r, color.g, color.b));
}

void SDL_FillRectA( SDL_Surface *dst, SDL_Rect *dstrect, Uint32 pixel, Uint8 alpha );

inline int SDL_FillRectA( SDL_Surface *dst, SDL_Rect *dstrect, const SDL_Color &color, Uint8 alpha )
{
	SDL_FillRectA(dst, dstrect, SDL_MapRGBA(dst->format, color.r, color.g, color.b, color.unused), alpha);
	return 0;
}


inline SDL_Color SDL_Color_RGBA( int r, int g, int b, int a = 0 )
{
	SDL_Color c = { r, g, b, a };
	return c;
}

#endif // SDL_EXT_HPP
