#ifndef LEVEL_SET_HPP
#define LEVEL_SET_HPP

#include "level.hpp"
#include "SDL.h"

void play_pack( SDL_Surface *surface, const std::string &directory );
void pack_done_screen( SDL_Surface *surface, const std::string &pack_name );

void level_screen( SDL_Surface *surface, const Level &level, const Highscore &highscore );

#endif // LEVEL_SET_HPP
