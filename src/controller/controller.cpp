/*  controller/controller.cpp
 *  
 *  Copyright 2010 Carl Reinke
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
 *  program; if not, see <http://www.nothinglost.net/licenses/MAME.txt>.
 */
#include "controller/controller.hpp"

using namespace std;

bool Controller::push_events = true;

const boost::array<SDLKey, Controller::functions_count> Controller::push_as_key = 
{{
	static_cast<SDLKey>(up_key),
	static_cast<SDLKey>(right_key),
	static_cast<SDLKey>(down_key),
	static_cast<SDLKey>(left_key),
	
	static_cast<SDLKey>(left_shoulder_key),
	static_cast<SDLKey>(right_shoulder_key),
	
	static_cast<SDLKey>(select_key),
	static_cast<SDLKey>(back_key),
	
	static_cast<SDLKey>(start_key),
	static_cast<SDLKey>(quit_key),
	
	static_cast<SDLKey>(vol_up_key),
	static_cast<SDLKey>(vol_down_key),
}};

Uint32 Controller::repeat_delay = SDL_DEFAULT_REPEAT_DELAY, Controller::repeat_interval = SDL_DEFAULT_REPEAT_INTERVAL;

vector< boost::shared_ptr<Controller> > controllers, disabled_controllers;

bool Controller::drop_input_enabled = false;

Controller::Controller( void )
{
	is_down.reset();
	was_down.reset();
	is_triggered.reset();
	
	fill(repeat_tick.begin(), repeat_tick.end(), 0);
	
	if (!drop_input_enabled)
	{
		SDL_SetEventFilter(&drop_input);
		
		drop_input_enabled = true;
	}
}

int Controller::drop_input( const SDL_Event *e )
{
	switch (e->type)
	{
	case SDL_KEYDOWN:
	case SDL_KEYUP:
	// case SDL_MOUSEMOTION:
	// case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
	case SDL_JOYAXISMOTION:
	case SDL_JOYBALLMOTION:
	case SDL_JOYHATMOTION:
	case SDL_JOYBUTTONDOWN:
	case SDL_JOYBUTTONUP:
		return 0;
	default:
		return 1;
	}
}

void Controller::update( void )
{
	was_down = is_down;
	update_down();
	
	is_triggered.reset();
	
	const Uint32 tick = SDL_GetTicks();
	
	for (int i = 0; i < functions_count; ++i)
	{
		if (is_down[i])
		{
			if (!was_down[i])
			{
				is_triggered[i] = true;
				repeat_tick[i] = tick + repeat_delay;
			}
			else if (tick > repeat_tick[i])  // if repeat delay expired
			{
				is_triggered[i] = true;
				repeat_tick[i] += repeat_interval;
			}
			
			if (is_triggered[i])
			{
				if (push_events)
					push_function_event(Functions(i));  // key down event
			}
		}
		else if (was_down[i])
		{
			if (push_events)
				push_function_event(Functions(i));  // key up event
		}
	}
}

void Controller::push_function_event( Functions function ) const
{
	SDL_Event event;
	event.type = is_down[function] ? SDL_KEYDOWN : SDL_KEYUP;
	event.key.type = event.type;
	event.key.state = is_down[function] ? SDL_PRESSED : SDL_RELEASED;
	event.key.keysym.scancode = 0;
	event.key.keysym.sym = push_as_key[function];
	event.key.keysym.mod = KMOD_NONE;
	event.key.keysym.unicode = 0;
	
	SDL_PushEvent(&event);
}
