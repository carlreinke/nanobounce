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
 *  program; if not, see <http://www.nothinglost.net/licenses/MAME.txt>.
 */
#ifndef CONTROLLER_CONTROLLER_HPP
#define CONTROLLER_CONTROLLER_HPP

#include "json/json.h"
#include "SDL.h"

class Controller
{
public:
	Controller( void );
	virtual ~Controller( void ) { }
	
	void update( void );
	virtual void tick_update( void ) { };
	
	virtual bool is_keyboard( void ) { return false; }
	
	typedef std::vector< boost::shared_ptr<Controller> > Set;
	
	enum Functions
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
		
		functions_count
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
	
	std::bitset<functions_count> is_down, was_down, is_triggered;
	
	static bool push_events;
	
	static Uint32 repeat_delay, repeat_interval;
	
protected:
	virtual void update_down( void ) = 0;
	
	static bool drop_input_enabled;
	static int drop_input( const SDL_Event *event );
	
	void push_function_event( Functions ) const;
	
private:
	static const boost::array<SDLKey, functions_count> push_as_key;
	
	boost::array<Uint32, functions_count> repeat_tick;
};

class ConfigurableController : public Controller
{
protected:
	void update_down( void );
	
	class Assignment
	{
	public:
		virtual int analog( const Controller & ) const;
		virtual bool digital( const Controller & ) const;
		
		virtual Json::Value serialize( void ) const = 0;
		virtual bool unserialize( const Json::Value & ) = 0;
		
		struct Cmp
		{
			bool operator()( const Assignment &lhs, const Assignment &rhs )
			{
				return (lhs.serialize() < rhs.serialize());
			}
			bool operator()( const boost::shared_ptr<Assignment> lhs, const boost::shared_ptr<Assignment> rhs )
			{
				return operator()(*lhs, *rhs);
			}
		};
		
		typedef std::set<boost::shared_ptr<Assignment>, Cmp> Set;
	};
	
	typedef std::vector<Assignment::Set> Assignments;
	boost::array<Assignments, functions_count> assignments;
	
	virtual const Json::Value &assignment_root( const Json::Value & ) const = 0;
	virtual boost::shared_ptr<Assignment> parse_assignment( const Json::Value & ) const = 0;
	
	void load_assignments( const std::string & = "controller.conf" );
};

extern Controller::Set controllers, disabled_controllers;

#endif // CONTROLLER_CONTROLLER_HPP
