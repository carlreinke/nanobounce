#ifndef MAIN_HPP
#define MAIN_HPP

#include "font.hpp"
#include "SDL.h"

enum
{
	USER_UPDATE,
	USER_FRAME
};

extern bool global_quit;

static const int fps = 40, ups = 40, ups_multiplier = 4;
extern int ms_per_update;

extern const std::string level_directory, music_directory, sample_directory, sprite_directory;

#endif // MAIN_HPP
