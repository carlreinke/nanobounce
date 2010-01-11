#ifndef VIDEO_HPP
#define VIDEO_HPP

#include "SDL.h"

extern int screen_width, screen_height, screen_bpp;
extern const Uint32 surface_flags;

SDL_Surface *init_video( void );

#endif // VIDEO_HPP
