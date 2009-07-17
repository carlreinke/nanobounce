#include "video.h"

using namespace std;

const int screen_width = 320, screen_height = 240;
int screen_bpp = 32;

SDL_Surface *init_video( void )
{
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) == -1)
	{
		cerr << SDL_GetError() << endl;
		exit(EXIT_FAILURE);
	}
	
	SDL_Surface *surface = SDL_SetVideoMode(320, 240, screen_bpp, SDL_HWSURFACE | SDL_ANYFORMAT | SDL_DOUBLEBUF);
	
	SDL_WM_SetCaption("Bouncing Frustration", NULL);
	SDL_ShowCursor(SDL_DISABLE);
	
	if (surface == NULL)
	{
		cerr << SDL_GetError() << endl;
		exit(EXIT_FAILURE);
	}
	
	screen_bpp = surface->format->BitsPerPixel;
	cout << "video: " << surface->w << "x" << surface->h << "x" << screen_bpp << endl;
	
	return surface;
}
