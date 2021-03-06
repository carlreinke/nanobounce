/*  nanobounce/src/sdl_ext.cpp
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
#include "sdl_ext.hpp"

template <typename T>
static inline Uint32 get_pixel( const void *pixels )
{
	return *reinterpret_cast<const T *>(pixels);
}

static inline Uint32 get_pixel24( const void *pixels )
{
	const Uint8 *pixels8 = reinterpret_cast<const Uint8 *>(pixels);
	
	Uint32 temp = *pixels8 << 16;
	temp |= *(++pixels8) << 8;
	temp |= *(++pixels8);
	
	return temp;
}

template <typename T>
static inline void set_pixel( void *pixels, Uint32 pixel )
{
	*reinterpret_cast<T *>(pixels) = pixel;
}

static inline void set_pixel24( void *pixels, Uint32 pixel )
{
	Uint8 *pixels8 = reinterpret_cast<Uint8 *>(pixels);
	
	*pixels8 = pixel >> 16;
	*(++pixels8) = pixel >> 8;
	*(++pixels8) = pixel;
}

static inline void crop_rect_to_surface( const SDL_Surface *surface, SDL_Rect *rect )
{
	if (rect->x > surface->w || rect->y > surface->h)
	{
		rect->w = 0;
		rect->h = 0;
		return;
	}
	
	Sint16 crop;
	
	crop = -rect->x;
	if (crop > 0)
	{
		rect->x = 0;
		if (crop > rect->w)
			rect->w = 0;
		else
			rect->w -= crop;
	}
	
	crop = -rect->y;
	if (crop > 0)
	{
		rect->y = 0;
		if (crop > rect->h)
			rect->h = 0;
		else
			rect->h -= crop;
	}
	
	crop = rect->x + rect->w - surface->w;
	if (crop > 0)
		rect->w -= crop;
	
	crop = rect->y + rect->h - surface->h;
	if (crop > 0)
		rect->h -= crop;
}

int SDL_FillRectA( SDL_Surface *dst, SDL_Rect *dstrect, Uint32 pixel, Uint8 alpha )
{
	if (alpha == SDL_ALPHA_OPAQUE)
		return SDL_FillRect(dst, dstrect, pixel);
	
	if (dstrect == NULL)
	{
		static SDL_Rect rect = { 0, 0, UINT16_MAX, UINT16_MAX };
		dstrect = &rect;
	}
	crop_rect_to_surface(dst, dstrect);
	
	if (SDL_MUSTLOCK(dst))
		SDL_LockSurface(dst);
	
	const Uint32 Rmask = dst->format->Rmask;
	const Uint8 Rshift = dst->format->Rshift;
	const Uint32 Gmask = dst->format->Gmask;
	const Uint8 Gshift = dst->format->Gshift;
	const Uint32 Bmask = dst->format->Bmask;
	const Uint8 Bshift = dst->format->Bshift;
	const Uint32 Amask = dst->format->Amask;
	const Uint32 dR = (pixel & Rmask) >> Rshift;
	const Uint32 dG = (pixel & Gmask) >> Gshift;
	const Uint32 dB = (pixel & Bmask) >> Bshift;
	
	Uint8 *pixels_row = reinterpret_cast<Uint8 *>(dst->pixels) + dst->pitch * dstrect->y + dst->format->BytesPerPixel * dstrect->x;
	
	switch (dst->format->BytesPerPixel)
	{
	case 4:
		for (Uint16 height = dstrect->h; height > 0; --height)
		{
			Uint8 *pixels = pixels_row;
			pixels_row += dst->pitch;
			
			for (Uint16 width = dstrect->w; width > 0; --width)
			{
				const Uint32 s = get_pixel<Uint32>(pixels);
				Uint32 sR = s & Rmask;
				Uint32 sG = s & Gmask;
				Uint32 sB = s & Bmask;
				const Uint32 sA = s & Amask;
				sR = (sR + ((((dR - (sR >> Rshift)) * alpha) >> 8) << Rshift)) & Rmask;
				sG = (sG + ((((dG - (sG >> Gshift)) * alpha) >> 8) << Gshift)) & Gmask;
				sB = (sB + ((((dB - (sB >> Bshift)) * alpha) >> 8) << Bshift)) & Bmask;
				set_pixel<Uint32>(pixels, sR | sG | sB | sA);
				
				pixels += 4;
			}
		}
		break;
		
	case 2:
		for (Uint16 height = dstrect->h; height > 0; --height)
		{
			Uint8 *pixels = pixels_row;
			pixels_row += dst->pitch;
			
			for (Uint16 width = dstrect->w; width > 0; --width)
			{
				const Uint32 s = get_pixel<Uint16>(pixels);
				Uint32 sR = s & Rmask;
				Uint32 sG = s & Gmask;
				Uint32 sB = s & Bmask;
				const Uint32 sA = s & Amask;
				sR = (sR + ((((dR - (sR >> Rshift)) * alpha) >> 8) << Rshift)) & Rmask;
				sG = (sG + ((((dG - (sG >> Gshift)) * alpha) >> 8) << Gshift)) & Gmask;
				sB = (sB + ((((dB - (sB >> Bshift)) * alpha) >> 8) << Bshift)) & Bmask;
				set_pixel<Uint16>(pixels, sR | sG | sB | sA);
				
				pixels += 2;
			}
		}
		break;
		
	default:
		assert(false);
		break;
	}
	
	if (SDL_MUSTLOCK(dst))
		SDL_UnlockSurface(dst);
	
	return 0;
}

void SDL_SetPixel( SDL_Surface *dst, int x, int y, Uint32 pixel )
{
	if (x < 0 || x >= dst->w || y < 0 || y >= dst->h)
		return;
	
	if (SDL_MUSTLOCK(dst))
		SDL_LockSurface(dst);
	
	Uint8 *pixels = reinterpret_cast<Uint8 *>(dst->pixels) + dst->pitch * y + dst->format->BytesPerPixel * x;
	
	switch (dst->format->BytesPerPixel)
	{
	case 4:
		set_pixel<Uint32>(pixels, pixel);
		break;
		
	case 2:
		set_pixel<Uint16>(pixels, pixel);
		break;
		
	default:
		assert(false);
		break;
	}
	
	if (SDL_MUSTLOCK(dst))
		SDL_UnlockSurface(dst);
}

void SDL_SetPixelA( SDL_Surface *dst, int x, int y, Uint32 pixel, Uint8 alpha )
{
	if (x < 0 || x >= dst->w || y < 0 || y >= dst->h)
		return;
	
	if (alpha == SDL_ALPHA_OPAQUE)
		return SDL_SetPixel(dst, x, y, pixel);
	
	if (SDL_MUSTLOCK(dst))
		SDL_LockSurface(dst);
	
	const Uint32 Rmask = dst->format->Rmask;
	const Uint8 Rshift = dst->format->Rshift;
	const Uint32 Gmask = dst->format->Gmask;
	const Uint8 Gshift = dst->format->Gshift;
	const Uint32 Bmask = dst->format->Bmask;
	const Uint8 Bshift = dst->format->Bshift;
	const Uint32 Amask = dst->format->Amask;
	const Uint32 dR = (pixel & Rmask) >> Rshift;
	const Uint32 dG = (pixel & Gmask) >> Gshift;
	const Uint32 dB = (pixel & Bmask) >> Bshift;
	
	Uint8 *pixels = reinterpret_cast<Uint8 *>(dst->pixels) + dst->pitch * y + dst->format->BytesPerPixel * x;
	
	Uint32 s;
	
	switch (dst->format->BytesPerPixel)
	{
	case 4:
		s = get_pixel<Uint32>(pixels);
		break;
		
	case 2:
		s = get_pixel<Uint16>(pixels);
		break;
		
	default:
		assert(false);
		break;
	}
	
	Uint32 sR = s & Rmask;
	Uint32 sG = s & Gmask;
	Uint32 sB = s & Bmask;
	const Uint32 sA = s & Amask;
	sR = (sR + ((((dR - (sR >> Rshift)) * alpha) >> 8) << Rshift)) & Rmask;
	sG = (sG + ((((dG - (sG >> Gshift)) * alpha) >> 8) << Gshift)) & Gmask;
	sB = (sB + ((((dB - (sB >> Bshift)) * alpha) >> 8) << Bshift)) & Bmask;
	set_pixel<Uint16>(pixels, sR | sG | sB | sA);
	
	switch (dst->format->BytesPerPixel)
	{
	case 4:
		set_pixel<Uint32>(pixels, sR | sG | sB | sA);
		break;
		
	case 2:
		set_pixel<Uint16>(pixels, sR | sG | sB | sA);
		break;
		
	default:
		assert(false);
		break;
	}
	
	if (SDL_MUSTLOCK(dst))
		SDL_UnlockSurface(dst);
}
