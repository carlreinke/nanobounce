#include "fixed.hpp"
#include "misc.hpp"
#include "sdl_ext.hpp"
#include "sprite.hpp"
#include "video.hpp"

using namespace std;

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

Sprite::Sprite( istream &netpbm )
: surface(NULL)
{
	load_ppm(netpbm);
}

Sprite::Sprite( const string &netpbm_file )
: surface(NULL)
{
	ifstream netpbm(netpbm_file.c_str());
	load_ppm(netpbm);
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

void Sprite::load_ppm( istream &ppm )
{
	destroy();
	
	string type;
	ppm >> type;
	if (type != "P3")
	{
		cerr << "error: sprite loading failed" << endl;
		return;
	}
	
	int w = get_no_comments<int>(ppm),
	    h = get_no_comments<int>(ppm),
	    component_max = get_no_comments<int>(ppm);
	
	surface = SDL_CreateRGBSurface(SDL_HWSURFACE || SDL_HWACCEL, w, h, screen_bpp, 0, 0, 0, 0);
	
	int x = 0, y = 0;
	
	while (ppm.good() && y < h)
	{
		SDL_Color color =
		{
			static_cast<int>(get_no_comments<int>(ppm) * (component_max / Fixed(255))),
			static_cast<int>(get_no_comments<int>(ppm) * (component_max / Fixed(255))),
			static_cast<int>(get_no_comments<int>(ppm) * (component_max / Fixed(255)))
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
		surface = SDL_CreateRGBSurface(SDL_HWSURFACE || SDL_HWACCEL, that.surface->h, that.surface->w, that.surface->format->BitsPerPixel, 0, 0, 0, 0);
		SDL_BlitSurface(that.surface, NULL, surface, NULL);
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

void Sprite::blit( SDL_Surface *surface, int x, int y, Uint8 alpha ) const
{
	if (this->surface != NULL)
	{
		SDL_Rect rect = { x, y, this->surface->w, this->surface->h };
		SDL_SetAlpha(this->surface, (alpha != SDL_ALPHA_OPAQUE) ? SDL_SRCALPHA : 0, alpha);
		SDL_BlitSurface(this->surface, NULL, surface, &rect);
	}
	else
	{
		SDL_Rect rect = { x, y, w, h };
		SDL_FillRectA(surface, &rect, color, alpha);
	}
}
