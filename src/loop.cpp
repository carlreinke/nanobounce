#include "controller.hpp"
#include "loop.hpp"
#include "main.hpp"
#include "volume.hpp"

using namespace std;

const uint fps = 35,
           ups = 70, ups_multiplier = 2;
const uint ms_per_frame = 1000 / fps,
           ms_per_update = 1000 / ups;

void Loop::loop( SDL_Surface *surface )
{
	this->surface = surface;
	
	loop_quit = false;
	
	fader.fade(Fader::in);
	
	static Uint32 update_ticks_ms = SDL_GetTicks(),
	              frame_ticks_ms = SDL_GetTicks();
	
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
					trigger_volume_change(Fixed(1) / 100);
					break;
				case Controller::vol_down_key:
					trigger_volume_change(-Fixed(1) / 100);
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
		
		Uint32 now_ticks_ms = SDL_GetTicks();
		
		if (now_ticks_ms > update_ticks_ms)
		{
			update_ticks_ms += ms_per_update;
			
			update();
			
			if (loop_quit)
				fader.fade(Fader::out);
			
			fader.update();
			done = fader.is_done() && fader.was_fading(Fader::out);
			
			update_volume_notification();
			
			int dropped = 0;
			while (now_ticks_ms > update_ticks_ms)
				update_ticks_ms += ms_per_update, ++dropped;
			if (dropped > 0)
				clog << "dropped " << dropped << " updates(s)" << endl;
		}
			
		if (now_ticks_ms > frame_ticks_ms)
		{
			frame_ticks_ms += ms_per_frame;
			
			draw(surface, fader.value());
			
			draw_volume_notification(surface);
			
			SDL_Flip(surface);
			
			int dropped = 0;
			while (now_ticks_ms > frame_ticks_ms)
				frame_ticks_ms += ms_per_frame, ++dropped;
			if (dropped > 0)
				clog << "dropped " << dropped << " frame(s)" << endl;
		}
		
		Sint32 wait_ticks_ms = min(update_ticks_ms, frame_ticks_ms) - SDL_GetTicks();
		if (wait_ticks_ms > 0)
			SDL_Delay(wait_ticks_ms);
	}
}

void Loop::handle_event( SDL_Event & )
{
	// nothing to do
}
