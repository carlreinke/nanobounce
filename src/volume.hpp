#ifndef VOLUME_HPP
#define VOLUME_HPP

#include "fixed.hpp"

#include <SDL.h>

void trigger_volume_change( Fixed delta );
void update_volume_notification( void );
void draw_volume_notification( SDL_Surface *surface );

#endif // VOLUME_HPP
