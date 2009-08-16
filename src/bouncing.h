#ifndef BOUNCING_H
#define BOUNCING_H

#include "font.h"
#include "SDL.h"

enum
{
	USER_FRAME,
	USER_UPDATE
};

extern int update_ticks;

extern std::map<int, Sprite> font_sprites;

extern bool global_quit;

#endif
