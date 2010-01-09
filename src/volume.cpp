#include "audio.hpp"
#include "controller.hpp"
#include "font.hpp"
#include "volume.hpp"

using namespace std;

static int show_volume_ticks = 0;
static ostringstream volume_text;

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
	
	cout << "volume: " << volume_text.str() << endl;
}

void update_volume_notification( void )
{
	if (show_volume_ticks > 0)
		--show_volume_ticks;
}

void draw_volume_notification( SDL_Surface *surface )
{
	if (show_volume_ticks > 0)
		font.blit(surface, 0, surface->h - font.height(font_sprites[3]), volume_text.str(), font_sprites[3], Font::left, 128);
}
