#include "controller/controller.hpp"
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
	
	static Uint32 next_update_ms = SDL_GetTicks(),
	              next_frame_ms = SDL_GetTicks();
	
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
		
		Uint32 now_ms = SDL_GetTicks();
		
		if (now_ms >= next_frame_ms)
		{
			if (now_ms >= next_frame_ms + ms_per_frame)
			{
				// aw, frame was too late
				int dropped = (now_ms - next_frame_ms) / ms_per_frame + 1;
				clog << "dropped " << dropped << " frame(s)" << endl;
				
				next_frame_ms += dropped * ms_per_frame;
			}
			else 
			{
				draw(surface, fader.value());
				
				draw_volume_notification(surface);
				
				SDL_Flip(surface);
				
				next_frame_ms += ms_per_frame;
			}
		}
		
		// process updates that will occur before next frame
		while (next_update_ms <= next_frame_ms)
		{
			update();
			
			if (loop_quit && !fader.was_fading(Fader::out))
				fader.fade(Fader::out);
			
			fader.update();
			done = fader.is_done() && fader.was_fading(Fader::out);
			
			update_volume_notification();
			
			next_update_ms += ms_per_update;
		}
		
		// wait until next frame
		Sint32 wait_ms = next_frame_ms - SDL_GetTicks();
		if (wait_ms > 0)
			SDL_Delay(wait_ms);
	}
}

void Loop::handle_event( SDL_Event & )
{
	// nothing to do
}
