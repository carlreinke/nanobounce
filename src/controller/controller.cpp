/*  controller/controller.cpp
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

bool Controller::push_events = true;

const boost::array<SDLKey, Controller::Control_COUNT> Controller::push_as_key = 
{{
	static_cast<SDLKey>(UP_KEY),
	static_cast<SDLKey>(RIGHT_KEY),
	static_cast<SDLKey>(DOWN_KEY),
	static_cast<SDLKey>(LEFT_KEY),
	
	static_cast<SDLKey>(LEFT_SHOULDER_KEY),
	static_cast<SDLKey>(RIGHT_SHOULDER_KEY),
	
	static_cast<SDLKey>(SELECT_KEY),
	static_cast<SDLKey>(BACK_KEY),
	
	static_cast<SDLKey>(START_KEY),
	static_cast<SDLKey>(QUIT_KEY),
	
	static_cast<SDLKey>(VOLUME_DOWN_KEY),
	static_cast<SDLKey>(VOLUME_UP_KEY),
}};

Uint32 Controller::repeat_delay = SDL_DEFAULT_REPEAT_DELAY;
Uint32 Controller::repeat_interval = SDL_DEFAULT_REPEAT_INTERVAL;

Controllers controllers;
Controllers disabled_controllers;

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
	
	for (int i = 0; i < Control_COUNT; ++i)
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
					push_control_event(Control(i));  // key down event
			}
		}
		else if (was_down[i])
		{
			if (push_events)
				push_control_event(Control(i));  // key up event
		}
	}
}

void Controller::push_control_event( Control control ) const
{
	SDL_Event event;
	event.type = is_down[control] ? SDL_KEYDOWN : SDL_KEYUP;
	event.key.type = event.type;
	event.key.state = is_down[control] ? SDL_PRESSED : SDL_RELEASED;
	event.key.keysym.scancode = 0;
	event.key.keysym.sym = push_as_key[control];
	event.key.keysym.mod = KMOD_NONE;
	event.key.keysym.unicode = 0;
	
	SDL_PushEvent(&event);
}

void ConfigurableController::update_down( void )
{
	for (uint i = 0; i < controls_mapping.size(); ++i)
	{
		is_down[i] = false;
		
		const ControlMapping &control_mapping = controls_mapping[i];
		
		for (uint j = 0; j < control_mapping.size(); ++j)
		{
			const CoincidentInputs &coincident_inputs = control_mapping[j];
			
			if (coincident_inputs.size() > 0)
			{
				bool coincident_inputs_down = true;
				
				for (const std::unique_ptr<Input> &input : coincident_inputs)
					coincident_inputs_down &= input->digital(*this);
				
				if (coincident_inputs_down)
					is_down[i] = true;
			}
		}
	}
}


int ConfigurableController::Input::analog( const Controller &controller ) const
{
	return (digital(controller) ? std::numeric_limits<Sint16>::max() : 0);
}

bool ConfigurableController::Input::digital( const Controller &controller ) const
{
	return (analog(controller) > std::numeric_limits<Sint16>::max() / 3);
}


bool ConfigurableController::load_controls_mapping( const std::string &conf_path )
{
	Json::Value root;
	Json::Reader reader;
	std::ifstream file(conf_path.c_str());
	bool success = reader.parse(file, root);
	
	if (!success)
	{
		std::cerr << "failed to parse '" << conf_path << "': " << reader.getFormatedErrorMessages();
		
		return false;
	}
	else
	{
		const Json::Value &config = get_config(root);
		
		for (uint i = 0; i < controls_mapping.size(); ++i)
		{
			ControlMapping &control_mapping = controls_mapping[i];
			const Json::Value &control_mapping_config = config[i];
			
			control_mapping.clear();
			control_mapping.resize(control_mapping_config.size());
			
			for (uint i = 0; i < control_mapping_config.size(); ++i)
			{
				CoincidentInputs &coincident_inputs = control_mapping[i];
				const Json::Value &coincident_inputs_config = control_mapping_config[i];
				
				for (uint i = 0; i < coincident_inputs_config.size(); ++i)
				{
					std::unique_ptr<Input> input = parse_input(coincident_inputs_config[i]);
					if (input.get() != NULL)
						coincident_inputs.push_back(std::move(input));
				}
			}
		}
	}
	
	return true;
}
