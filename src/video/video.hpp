/*  video/video.hpp
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
#ifndef VIDEO_HPP
#define VIDEO_HPP

#include "SDL.h"

extern int screen_width, screen_height, screen_bpp;
extern const Uint32 surface_flags;
extern int video_scale;

SDL_Surface *init_video( bool fullscreen );

#ifndef TARGET_GP2X
void scale_and_flip( SDL_Surface *surface );
#else
#define scale_and_flip SDL_Flip
#endif

#endif // VIDEO_HPP
