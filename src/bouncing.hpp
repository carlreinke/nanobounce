#ifndef BOUNCING_HPP
#define BOUNCING_HPP

#include "font.hpp"
#include "SDL.h"

enum
{
	USER_FRAME,
	USER_UPDATE
};

extern int ms_per_update;

extern std::map<int, Sprite> font_sprites;

extern bool global_quit;

#endif // BOUNCING_HPP
