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
 *  program; if not, see <http://www.intoorbit.org/licenses/MAME.txt>.
 */
#include "controller/controller.hpp"

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
	
	static_cast<SDLKey>(vol_down_key),
	static_cast<SDLKey>(vol_up_key),
}};

Uint32 Controller::repeat_delay = SDL_DEFAULT_REPEAT_DELAY, Controller::repeat_interval = SDL_DEFAULT_REPEAT_INTERVAL;

Controller::Set controllers, disabled_controllers;

bool Controller::drop_input_enabled = false;

Controller::Controller( void )
{
	is_down.reset();
	was_down.reset();
	is_triggered.reset();
	
	std::fill(repeat_tick.begin(), repeat_tick.end(), 0);
	
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

void ConfigurableController::update_down( void )
{
	for (uint i = 0; i < assignments.size(); ++i)
	{
		is_down[i] = false;
		
		Assignments &function = assignments[i];
		
		for (uint j = 0; j < function.size(); ++j)
		{
			Assignment::Set &function_assignments = function[j];
			
			if (function_assignments.size() > 0)
			{
				bool temp = true;
				
				for (const std::shared_ptr<Assignment> &function_assignment : function_assignments)
					temp &= function_assignment->digital(*this);
				
				if (temp)
					is_down[i] = true;
			}
		}
	}
}


int ConfigurableController::Assignment::analog( const Controller &controller ) const
{
	return (digital(controller) ? std::numeric_limits<Sint16>::max() : 0);
}

bool ConfigurableController::Assignment::digital( const Controller &controller ) const
{
	return (analog(controller) > std::numeric_limits<Sint16>::max() / 3);
}


void ConfigurableController::load_assignments( const std::string &conf_path )
{
	Json::Value root;
	Json::Reader reader;
	std::ifstream file(conf_path.c_str());
	bool success = reader.parse(file, root);
	
	if (!success)
	{
		std::cerr << "failed to parse '" << conf_path << "': " << reader.getFormatedErrorMessages();
	}
	else
	{
		const Json::Value &controller_root = assignment_root(root);
		
		for (uint i = 0; i < assignments.size(); ++i)
		{
			Assignments &function = assignments[i];
			const Json::Value &function_config = controller_root[i];
			
			function.resize(function_config.size());
			
			for (uint i = 0; i < function_config.size(); ++i)
			{
				Assignment::Set &function_assignments = function[i];
				const Json::Value &function_assignments_config = function_config[i];
				
				for (uint i = 0; i < function_assignments_config.size(); ++i)
				{
					std::shared_ptr<Assignment> temp = parse_assignment(function_assignments_config[i]);
					if (temp.get() != NULL)
						function_assignments.insert(temp);
				}
			}
		}
	}
}
