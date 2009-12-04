#include "video.hpp"

using namespace std;

int screen_width = 320, screen_height = 240, screen_bpp = 32;

SDL_Surface *init_video( void )
{
	SDL_Surface *surface = SDL_SetVideoMode(screen_width, screen_height, screen_bpp, SDL_HWSURFACE | SDL_ANYFORMAT | SDL_DOUBLEBUF);
	
	if (surface == NULL)
	{
		cerr << SDL_GetError() << endl;
		exit(EXIT_FAILURE);
	}
	
	SDL_WM_SetCaption("Nanobounce", NULL);
	SDL_ShowCursor(SDL_DISABLE);
	
	screen_width = surface->w;
	screen_height = surface->h;
	screen_bpp = surface->format->BitsPerPixel;
	
	clog << "video: " << screen_width << "x" << screen_height << "x" << screen_bpp << endl;
	
	return surface;
}

Fader::Fader( int step )
: done(true),
  alpha(SDL_ALPHA_TRANSPARENT), target(alpha), step(step)
{
	// good to go
}

void Fader::fade( Fader::Direction direction )
{
	done = false;
	target = static_cast<int>(direction);
	step = (target < alpha) ? -abs(step) : abs(step);
}

void Fader::update( void )
{
	if (!done)
	{
		alpha = min(max(SDL_ALPHA_TRANSPARENT, alpha + step), SDL_ALPHA_OPAQUE);
		done = (alpha == target);
	}
}
