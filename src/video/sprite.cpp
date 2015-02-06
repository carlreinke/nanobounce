/*  video/sprite.cpp
 *  
 *  Copyright 2010 Carl Reinke
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
#include "misc.hpp"
#include "sdl_ext.hpp"
#include "video/sprite.hpp"
#include "video/video.hpp"

Sprite::Sprite( void )
: surface(NULL), w(0), h(0)
{
	// good to go
}

Sprite::Sprite( uint width, uint height, const SDL_Color &color )
: surface(NULL), color(color), w(width), h(height)
{
	assert(width > 0 && height > 0);
}

Sprite::Sprite( std::istream &netpbm )
: surface(NULL)
{
	load_ppm(netpbm);
}

Sprite::Sprite( const std::string &netpbm_file )
: surface(NULL)
{
	std::ifstream netpbm(netpbm_file.c_str());
	load_ppm(netpbm);
}

Sprite::Sprite( const Sprite &that, const SDL_Rect &rect )
{
	surface = SDL_CreateRGBSurface(surface_flags, rect.w, rect.h, screen_bpp, 0, 0, 0, 0);
	
	that.blit(surface, -rect.x, -rect.y);
}

Sprite::Sprite( const Sprite &that )
: surface(NULL)
{
	copy(that);
}

Sprite::~Sprite( void )
{
	destroy();
}

Sprite & Sprite::operator=( const Sprite &that )
{
	if (this != &that)
	{
		destroy();
		copy(that);
	}
	return *this;
}

void Sprite::load_ppm( std::istream &ppm )
{
	destroy();
	
	std::string type;
	ppm >> type;
	if (type != "P3")
	{
		std::cerr << "error: sprite loading failed" << std::endl;
		return;
	}
	
	w = get_no_comments<uint>(ppm);
	h = get_no_comments<uint>(ppm);
	int component_max = get_no_comments<int>(ppm);
	
	surface = SDL_CreateRGBSurface(surface_flags, w, h, screen_bpp, 0, 0, 0, 0);
	
	uint x = 0, y = 0;
	
	while (ppm.good() && y < h)
	{
		SDL_Color color =
		{
			static_cast<Uint8>(get_no_comments<uint>(ppm) * 255 / component_max),
			static_cast<Uint8>(get_no_comments<uint>(ppm) * 255 / component_max),
			static_cast<Uint8>(get_no_comments<uint>(ppm) * 255 / component_max),
		};
		
		SDL_SetPixel(surface, x, y, color);
		
		if (++x == w)
		{
			x = 0;
			++y;
		}
	}
}

void Sprite::copy( const Sprite &that )
{
	if (that.surface != NULL)
	{
		surface = SDL_DuplicateRGBSurface(that.surface);
	}
	else
	{
		color = that.color;
		w = that.w;
		h = that.h;
	}
}

void Sprite::destroy( void )
{
	if (surface != NULL)
	{
		SDL_FreeSurface(surface);
		surface = NULL;
	}
}

void Sprite::blit( SDL_Surface *surface, Sint16 x, Sint16 y, Uint8 alpha ) const
{
	if (this->surface != NULL)
	{
		SDL_Rect rect = { x, y, static_cast<Uint16>(this->surface->w), static_cast<Uint16>(this->surface->h) };
		SDL_SetAlpha(this->surface, (alpha != SDL_ALPHA_OPAQUE) ? SDL_SRCALPHA : 0, alpha);
		SDL_BlitSurface(this->surface, NULL, surface, &rect);
	}
	else
	{
		SDL_Rect rect = { x, y, static_cast<Uint16>(w), static_cast<Uint16>(h) };
		SDL_FillRectA(surface, &rect, color, alpha);
	}
}
