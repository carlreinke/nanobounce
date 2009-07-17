#ifndef VIDEO_H
#define VIDEO_H

#include "SDL.h"

extern const int screen_width, screen_height;
extern int screen_bpp;

SDL_Surface *init_video( void );

#endif
