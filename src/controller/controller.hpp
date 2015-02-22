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
	
	enum Control
	{
		UP = 0,
		RIGHT,
		DOWN,
		LEFT,
		
		LEFT_SHOULDER,
		RIGHT_SHOULDER,
		
		SELECT,
		BACK,
		
		START,
		QUIT,
		
		VOLUME_DOWN,
		VOLUME_UP,
		
		Control_COUNT
	};
	
	enum Key
	{
		UP_KEY             = SDLK_UP,
		RIGHT_KEY          = SDLK_RIGHT,
		DOWN_KEY           = SDLK_DOWN,
		LEFT_KEY           = SDLK_LEFT,
		
		LEFT_SHOULDER_KEY  = SDLK_LEFTBRACKET,
		RIGHT_SHOULDER_KEY = SDLK_RIGHTBRACKET,
		
		SELECT_KEY         = SDLK_SPACE,
		BACK_KEY           = SDLK_BACKSPACE,
		
		START_KEY          = SDLK_RETURN,
		QUIT_KEY           = SDLK_ESCAPE,
		
		VOLUME_DOWN_KEY    = SDLK_MINUS,
		VOLUME_UP_KEY      = SDLK_PLUS
	};
	
	std::bitset<Control_COUNT> is_down;
	std::bitset<Control_COUNT> was_down;
	std::bitset<Control_COUNT> is_triggered;
	
	static bool push_events;
	
	static Uint32 repeat_delay;
	static Uint32 repeat_interval;
	
protected:
	virtual void update_down( void ) = 0;
	
	static bool drop_input_enabled;
	static int drop_input( const SDL_Event *event );
	
	void push_control_event( Control ) const;
	
private:
	static const boost::array<SDLKey, Control_COUNT> push_as_key;
	
	boost::array<Uint32, Control_COUNT> repeat_tick;
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
	boost::array<ControlMapping, Control_COUNT> controls_mapping;
	
	virtual const Json::Value &get_config( const Json::Value & ) const = 0;
	virtual std::unique_ptr<Input> parse_input( const Json::Value & ) const = 0;
	
public:
	bool load_controls_mapping( const std::string & );
};

typedef std::vector< std::shared_ptr<Controller> > Controllers;

extern Controllers controllers;
extern Controllers disabled_controllers;

#endif // CONTROLLER_CONTROLLER_HPP
