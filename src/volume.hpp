/*  nanobounce/src/volume.hpp
 *  
 *  Copyright 2015 Carl Reinke
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
 *  program; if not, see <http://www.intoorbit.org/licenses/MAME.txt>.
 */
#ifndef VOLUME_HPP
#define VOLUME_HPP

#include "fixed.hpp"

#include <SDL.h>

void trigger_volume_change( Fixed delta );
void update_volume_notification( void );
void draw_volume_notification( SDL_Surface *surface );

#endif // VOLUME_HPP
