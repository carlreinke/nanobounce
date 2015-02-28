/*  nanobounce/src/loop.cpp
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
#include "controller/controller.hpp"
#include "loop.hpp"
#include "main.hpp"
#include "volume.hpp"
#include "video/video.hpp"

const uint frame_per_second_limit = 60;

void Loop::loop( SDL_Surface *surface )
{
	this->surface = surface;
	
	loop_quit = false;
	
	fader.fade(Fader::IN);
	
	bool done = false;
	
	while (!done && !global_quit)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				global_quit = true;
				break;
				
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym)
				{
				case Controller::VOLUME_UP_KEY:
					trigger_volume_change(make_frac<Fixed>(1, 100));
					break;
				case Controller::VOLUME_DOWN_KEY:
					trigger_volume_change(make_frac<Fixed>(-1, 100));
					break;
					
				default:
					if (!loop_quit)  // ignore input during fade-out
						handle_event(e);
					break;
				}
				break;
				
			default:
				handle_event(e);
				break;
			}
		}
		
		static Uint32 last_update_msec = SDL_GetTicks();
		const Uint32 now_msec = SDL_GetTicks();
		
		while (last_update_msec < now_msec)
		{
			update();
			
			if (loop_quit && !fader.was_fading(Fader::OUT))
				fader.fade(Fader::OUT);
			
			fader.update();
			done = fader.is_done() && fader.was_fading(Fader::OUT);
			
			update_volume_notification();
			
			// prevent too much jolt
			if ((now_msec - last_update_msec) / msec_per_update >= update_per_sec / 4)
				last_update_msec = now_msec;
			else
				last_update_msec += msec_per_update;
		}
		
		static uint delay_msec = 1000 / frame_per_second_limit;
		
		// frame limiting
		{
			static Uint32 last_fps_msec = SDL_GetTicks();
			static uint frames = 0;
			const uint fps_refresh = 1000;
			
			++frames;
			if (now_msec - last_fps_msec > fps_refresh)
			{
				uint frame_per_second = frames * (now_msec - last_fps_msec) / fps_refresh;
				
				// std::cout << frame_per_second << " fps (" << delay_msec << " ms delay)" << std::endl;
				
				if (frame_per_second > frame_per_second_limit)
					delay_msec += fps_refresh;
				else if (delay_msec > fps_refresh)
					delay_msec -= fps_refresh;
				
				frames = 0;
				last_fps_msec = now_msec;
			}
		}
		
		// draw frame
		{
			draw(surface, fader.value());
			
			draw_volume_notification(surface);
			
			scale_and_flip(surface);
		}
		
		SDL_Delay(delay_msec);
	}
}

void Loop::handle_event( SDL_Event & )
{
	// nothing to do
}
