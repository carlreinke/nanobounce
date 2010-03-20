#include "controller/controller.hpp"
#include "loop.hpp"
#include "main.hpp"
#include "volume.hpp"
#include "video/video.hpp"

using namespace std;

const uint frame_per_second_limit = 60;

void Loop::loop( SDL_Surface *surface )
{
	this->surface = surface;
	
	loop_quit = false;
	
	fader.fade(Fader::in);
	
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
				case Controller::vol_up_key:
					trigger_volume_change(make_frac<Fixed>(1, 100));
					break;
				case Controller::vol_down_key:
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
		
		static Uint32 last_update_usec = SDL_GetTicks() * 1000;
		const Uint32 now_msec = SDL_GetTicks(),
		             now_usec = now_msec * 1000;
		
		while (last_update_usec < now_usec)
		{
			update();
			
			if (loop_quit && !fader.was_fading(Fader::out))
				fader.fade(Fader::out);
			
			fader.update();
			done = fader.is_done() && fader.was_fading(Fader::out);
			
			update_volume_notification();
			
			// prevent too much jolt
			if ((now_usec - last_update_usec) / usec_per_update >= update_per_sec / 4)
				last_update_usec = now_usec;
			else
				last_update_usec += usec_per_update;
		}
		
		static uint delay_usec = 1000000 / frame_per_second_limit;
		
		// frame limiting
		{
			static Uint32 last_fps_msec = SDL_GetTicks();
			static uint frames = 0;
			const uint fps_refresh = 1000;
			
			++frames;
			if (now_msec - last_fps_msec > fps_refresh)
			{
				uint frame_per_second = frames * (now_msec - last_fps_msec) / fps_refresh;
				
				// cout << frame_per_second << " fps (" << delay_usec << " usec delay)" << endl;
				
				if (frame_per_second > frame_per_second_limit)
					delay_usec += fps_refresh;
				else if (delay_usec > fps_refresh)
					delay_usec -= fps_refresh;
				
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
		
#ifdef TARGET_GP2X
		usleep(delay_usec);
#else
		SDL_Delay(delay_usec / 1000);
#endif
	}
}

void Loop::handle_event( SDL_Event & )
{
	// nothing to do
}
