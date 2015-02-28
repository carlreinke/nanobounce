/*  nanobounce/src/volume.cpp
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
#include "audio/audio.hpp"
#include "controller/controller.hpp"
#include "video/font.hpp"
#include "volume.hpp"

using std::max;
using std::min;

static int show_volume_ticks = 0;
static std::ostringstream volume_text;

void trigger_volume_change( Fixed delta )
{
	show_volume_ticks = 20;
	
	if (volume == 0 && delta < 0)
	{
		switch (++audio_mode)
		{
		case NO_SAMPLES:
			volume_text.str("SAMPLES OFF");
			break;
		case NO_MUSIC:
			volume_text.str("MUSIC OFF");
			break;
		case ALL_AUDIO:
			volume_text.str("ALL ON");
			break;
		}
	}
	else
	{
		volume = min(max(Fixed(0), volume + delta), Fixed(1));
		
		volume_text.str("");
		volume_text << static_cast<int>(volume * 100 + 0.5f) << "%";
	}
	
	std::cout << "volume: " << volume_text.str() << std::endl;
}

void update_volume_notification( void )
{
	if (show_volume_ticks > 0)
		--show_volume_ticks;
}

void draw_volume_notification( SDL_Surface *surface )
{
	if (show_volume_ticks > 0)
		font.blit(surface, 0, surface->h - font.height(font_sprites[3]), volume_text.str(), font_sprites[3], Font::LEFT, 128);
}
