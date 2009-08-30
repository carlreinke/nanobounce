#ifndef VIDEO_HPP
#define VIDEO_HPP

#include "SDL.h"

extern const int screen_width, screen_height;
extern int screen_bpp;

SDL_Surface *init_video( void );

#endif // VIDEO_HPP
