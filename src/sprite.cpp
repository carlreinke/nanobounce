#include "fixed.h"
#include "misc.h"
#include "sprite.h"
#include "video.h"

using namespace std;

Sprite::Sprite( void )
{
	surface = SDL_CreateRGBSurface(SDL_HWSURFACE || SDL_HWACCEL, 0, 0, screen_bpp, 0, 0, 0, 0);
}

Sprite::Sprite( unsigned int width, unsigned int height, const SDL_Color &color )
{
	surface = SDL_CreateRGBSurface(SDL_HWSURFACE || SDL_HWACCEL, width, height, screen_bpp, 0, 0, 0, 0);
	SDL_FillRect(surface, NULL, color);
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
	
	SDL_Rect rect = { 0, 0, 1, 1 };
	
	while (!ppm.eof() && rect.y < h)
	{
		SDL_Color color =
		{
			(int)(get_no_comments<int>(ppm) * (component_max / (Fixed)255)),
			(int)(get_no_comments<int>(ppm) * (component_max / (Fixed)255)),
			(int)(get_no_comments<int>(ppm) * (component_max / (Fixed)255))
		};
		
		SDL_FillRect(surface, &rect, color);
		
		if (++rect.x == w)
		{
			rect.x = 0;
			++rect.y;
		}
	}
}

void Sprite::copy( const Sprite &that )
{
	surface = SDL_CreateRGBSurface(SDL_HWSURFACE || SDL_HWACCEL, that.surface->h, that.surface->w, that.surface->format->BitsPerPixel, 0, 0, 0, 0);
	SDL_BlitSurface(that.surface, NULL, surface, NULL);
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
	SDL_Rect rect = { x, y, this->surface->w, this->surface->h };
	SDL_SetAlpha(this->surface, (alpha != SDL_ALPHA_OPAQUE) ? SDL_SRCALPHA : 0, alpha);
	SDL_BlitSurface(this->surface, NULL, surface, &rect);
}
