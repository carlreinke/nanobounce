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

#ifndef TARGET_GP2X
int video_scale = 2;
#else
int video_scale = 1;
#endif

SDL_Surface *init_video( bool fullscreen )
{
	SDL_InitSubSystem(SDL_INIT_VIDEO);
	
	const Uint32 video_flags = surface_flags | SDL_DOUBLEBUF | (fullscreen ? SDL_FULLSCREEN : 0);
	
	screen_bpp = SDL_VideoModeOK(screen_width * video_scale, screen_height * video_scale, screen_bpp, video_flags);
	if (screen_bpp == 24)
		screen_bpp = 32;
	
	SDL_Surface *surface = SDL_SetVideoMode(screen_width * video_scale, screen_height * video_scale, screen_bpp, video_flags);
	
	if (video_scale > 1 && surface != NULL)
		surface = SDL_CreateRGBSurface(surface_flags, screen_width, screen_height, screen_bpp, 0, 0, 0, 0);
	
	if (surface == NULL)
	{
		cerr << SDL_GetError() << endl;
		exit(EXIT_FAILURE);
	}
	
	screen_width = surface->w;
	screen_height = surface->h;
	
	cout << "video: " << screen_width << "x" << screen_height << "x" << screen_bpp << endl;
	if (video_scale > 1)
		cout << "shadow video: " << SDL_GetVideoSurface()->w << "x" << SDL_GetVideoSurface()->h << "x" << screen_bpp << endl;
	
	SDL_WM_SetCaption("Nanobounce", NULL);
	
	SDL_ShowCursor(SDL_DISABLE);
	
	return surface;
}

template <int Bpp, typename T>
static void nn( SDL_Surface *src_surface, SDL_Surface *dst_surface );

void scale_and_flip( SDL_Surface *surface )
{
	SDL_Surface *video_surface = SDL_GetVideoSurface();
	
	switch (screen_bpp)
	{
	case 32:
		nn<4, Uint32>(surface, video_surface);
		break;
	case 16:
		nn<2, Uint16>(surface, video_surface);
		break;
	default:
		assert(false);
		break;
	}
	
	SDL_Flip(video_surface);
}

template <int Bpp, typename T>
static void nn( SDL_Surface *src_surface, SDL_Surface *dst_surface )
{
	Uint8 *src = reinterpret_cast<Uint8 *>(src_surface->pixels), *src_temp,
	      *dst = reinterpret_cast<Uint8 *>(dst_surface->pixels), *dst_temp;
	const int src_pitch = src_surface->pitch,
	          dst_pitch = dst_surface->pitch;
	
	const int height = screen_height, // src_surface->h
	          width = screen_width,   // src_surface->w
	          scale = min(dst_surface->w / width, dst_surface->h / height);
	
	for (int y = height; y > 0; y--)
	{
		src_temp = src;
		dst_temp = dst;
		
		for (int x = width; x > 0; x--)
		{
			for (int z = scale; z > 0; z--)
			{
				*reinterpret_cast<T *>(dst) = *reinterpret_cast<T *>(src);
				dst += Bpp;
			}
			src += Bpp;
		}
		
		src = src_temp + src_pitch;
		dst = dst_temp + dst_pitch;
		
		for (int z = scale; z > 1; z--)
		{
			memcpy(dst, dst_temp, dst_pitch);
			dst += dst_pitch;
		}
	}
}
