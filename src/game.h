#ifndef GAME_H
#define GAME_H

#include "highscore.h"
#include "level.h"
#include "SDL.h"

void play_pack( SDL_Surface *surface, const std::string &directory );
void pack_done_screen( SDL_Surface *surface, const std::string &pack_name );

void level_screen( SDL_Surface *surface, const Level &level, const Highscore &highscore );
void level_loop( SDL_Surface *surface, Level &level, Highscore &new_highscore );

#endif
