/*  controller/controller.hpp
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
#ifndef CONTROLLER_CONTROLLER_HPP
#define CONTROLLER_CONTROLLER_HPP

#include "misc.hpp"
#include "json/json.h"

#include <SDL.h>

class Controller
{
public:
	Controller( void );
	virtual ~Controller( void ) { }
	
	void update( void );
	virtual void tick_update( void ) { };
	
	virtual bool is_keyboard( void ) { return false; }
	
	typedef std::vector< std::shared_ptr<Controller> > Set;
	
	enum Control
	{
		up = 0,
		right,
		down,
		left,
		
		left_shoulder,
		right_shoulder,
		
		select,
		back,
		
		start,
		quit,
		
		vol_down,
		vol_up,
		
		control_count
	};
	enum Keys
	{
		up_key             = SDLK_UP,
		right_key          = SDLK_RIGHT,
		down_key           = SDLK_DOWN,
		left_key           = SDLK_LEFT,
		
		left_shoulder_key  = SDLK_LEFTBRACKET,
		right_shoulder_key = SDLK_RIGHTBRACKET,
		
		select_key         = SDLK_SPACE,
		back_key           = SDLK_BACKSPACE,
		
		start_key          = SDLK_RETURN,
		quit_key           = SDLK_ESCAPE,
		
		vol_down_key       = SDLK_MINUS,
		vol_up_key         = SDLK_PLUS
	};
	
	std::bitset<control_count> is_down, was_down, is_triggered;
	
	static bool push_events;
	
	static Uint32 repeat_delay, repeat_interval;
	
protected:
	virtual void update_down( void ) = 0;
	
	static bool drop_input_enabled;
	static int drop_input( const SDL_Event *event );
	
	void push_control_event( Control ) const;
	
private:
	static const boost::array<SDLKey, control_count> push_as_key;
	
	boost::array<Uint32, control_count> repeat_tick;
};

class ConfigurableController : public Controller
{
protected:
	void update_down( void );
	
	class Input
	{
	public:
		virtual int analog( const Controller & ) const;
		virtual bool digital( const Controller & ) const;
		
		virtual Json::Value serialize( void ) const = 0;
		virtual bool unserialize( const Json::Value & ) = 0;
	};
	
	typedef std::vector<std::unique_ptr<Input>> CoincidentInputs;
	typedef std::vector<CoincidentInputs> ControlMapping;
	boost::array<ControlMapping, control_count> controls_mapping;
	
	virtual const Json::Value &get_config( const Json::Value & ) const = 0;
	virtual std::unique_ptr<Input> parse_input( const Json::Value & ) const = 0;
	
	void load_controls_mapping( const std::string & = "controller.conf" );
};

extern Controller::Set controllers, disabled_controllers;

#endif // CONTROLLER_CONTROLLER_HPP
