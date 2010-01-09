#include "controller.hpp"
#include "loop.hpp"
#include "main.hpp"
#include "volume.hpp"

using namespace std;

void Loop::loop( SDL_Surface *surface )
{
	this->surface = surface;
	
	loop_quit = false;
	
	fader.fade(Fader::in);
	
	bool done = false;
	
	while (!done && !global_quit)
	{
		SDL_WaitEvent(NULL);
		
		int updates = 0, frames = 0;
		
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
				
			case SDL_USEREVENT:
				switch (e.user.code)
				{
				case USER_UPDATE:
					++updates;
					break;
				case USER_FRAME:
					++frames;
					break;
				}
				break;
				
			default:
				handle_event(e);
				break;
			}
		}
		
		while (updates--)
		{
			update();
			
			if (loop_quit)
				fader.fade(Fader::out);
			
			fader.update();
			done = fader.is_done() && fader.was_fading(Fader::out);
			
			update_volume_notification();
		}
		
		if (frames--)
		{
			draw(surface, fader.value());
			
			draw_volume_notification(surface);
			
			SDL_Flip(surface);
			
			if (frames > 0)
				clog << "dropped " << frames << " frame(s)" << endl;
		}
	}
}

void Loop::handle_event( SDL_Event & )
{
	// nothing to do
}
