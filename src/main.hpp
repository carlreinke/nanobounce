#ifndef MAIN_HPP
#define MAIN_HPP

#include "font.hpp"
#include "SDL.h"

enum
{
	USER_UPDATE,
	USER_FRAME
};

extern int ms_per_update;

extern std::map<int, Sprite> font_sprites;

extern bool global_quit;

extern const std::string level_directory, music_directory, sample_directory, sprite_directory;

#endif // MAIN_HPP
