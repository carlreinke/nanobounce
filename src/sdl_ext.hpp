#ifndef SDL_EXT_HPP
#define SDL_EXT_HPP

#include "video/video.hpp"
#include "SDL.h"

class SDL_Surface_smartptr
{
public:
	SDL_Surface_smartptr( int w = 0, int h = 0, int bpp = 0 ) : surface(NULL) { resize(w, h, bpp); }
	~SDL_Surface_smartptr( void ) { SDL_FreeSurface(surface); }
	
	SDL_Surface &operator*( void ) const { return *surface; }
	SDL_Surface *operator->( void ) const { return surface; }
	SDL_Surface *get( void ) const { return surface; }
	
	void resize( int w, int h, int bpp )
	{
		SDL_FreeSurface(surface);
		surface = SDL_CreateRGBSurface(surface_flags, w, h, bpp, 0, 0, 0, 0);
	}
	
private:
	SDL_Surface *surface;
};

inline SDL_Surface *SDL_DuplicateRGBSurface( SDL_Surface *src )
{
	SDL_Surface *dst = SDL_CreateRGBSurface(surface_flags, src->w, src->h, src->format->BitsPerPixel, src->format->Rmask, src->format->Gmask, src->format->Bmask, src->format->Amask);
	if (dst != NULL)
	{
		SDL_SetAlpha(src, 0, 0);
		SDL_BlitSurface(src, NULL, dst, NULL);
	}
	return dst;
}

inline SDL_Color SDL_Color_RGBA( int r, int g, int b, int a = 0 )
{
	SDL_Color c = { r, g, b, a };
	return c;
}

inline Uint32 SDL_MapRGB( SDL_PixelFormat *fmt, const SDL_Color &color )
{
	return SDL_MapRGB(fmt, color.r, color.g, color.b);
} 

inline Uint32 SDL_MapRGBA( SDL_PixelFormat *fmt, const SDL_Color &color )
{
	return SDL_MapRGBA(fmt, color.r, color.g, color.b, color.unused);
} 

// rectangle

inline int SDL_FillRect( SDL_Surface *dst, SDL_Rect *dstrect, const SDL_Color &color )
{
	return SDL_FillRect(dst, dstrect, SDL_MapRGB(dst->format, color));
}

void SDL_FillRectA( SDL_Surface *dst, SDL_Rect *dstrect, Uint32 pixel, Uint8 alpha );

inline int SDL_FillRectA( SDL_Surface *dst, SDL_Rect *dstrect, const SDL_Color &color, Uint8 alpha )
{
	SDL_FillRectA(dst, dstrect, SDL_MapRGBA(dst->format, color), alpha);
	return 0;
}

// pixel

void SDL_SetPixel( SDL_Surface *dst, int x, int y, Uint32 pixel );

inline void SDL_SetPixel( SDL_Surface *dst, int x, int y, const SDL_Color &color )
{
	SDL_SetPixel(dst, x, y, SDL_MapRGB(dst->format, color));
}

void SDL_SetPixelA( SDL_Surface *dst, int x, int y, Uint32 pixel, Uint8 alpha );

inline void SDL_SetPixelA( SDL_Surface *dst, int x, int y, const SDL_Color &color, Uint8 alpha )
{
	SDL_SetPixelA(dst, x, y, SDL_MapRGBA(dst->format, color), alpha);
}

#endif // SDL_EXT_HPP
