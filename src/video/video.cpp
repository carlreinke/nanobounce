/*  video/video.cpp
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
 *  program; if not, see <http://www.nothinglost.net/licenses/MAME.txt>.
 */
#include "video/video.hpp"

using namespace std;

int screen_width = 320, screen_height = 240, screen_bpp = 32;
const Uint32 surface_flags = SDL_HWSURFACE;

SDL_Surface *init_video( bool fullscreen )
{
	const Uint32 video_flags = surface_flags | SDL_ANYFORMAT | SDL_DOUBLEBUF | (fullscreen ? SDL_FULLSCREEN : 0);
	SDL_Surface *surface = SDL_SetVideoMode(screen_width, screen_height, screen_bpp, video_flags);
	
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
	
	cout << "video: " << screen_width << "x" << screen_height << "x" << screen_bpp << endl;
	
	return surface;
}
