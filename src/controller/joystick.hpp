/*  controller.hpp
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
#ifndef CONTROLLER_JOYSTICK_HPP
#define CONTROLLER_JOYSTICK_HPP

#include "controller/controller.hpp"

class Joystick : public ConfigurableController
{
public:
	Joystick( int i = 0 );
	~Joystick( void );
	
	bool is_joystick( void ) { return true; }
	
private:
	SDL_Joystick *joystick;
	
	const Json::Value &assignment_root( const Json::Value & ) const;
	boost::shared_ptr<Assignment> parse_assignment( const Json::Value & ) const;
	
	class Button : public Assignment
	{
	public:
		bool digital( const Controller & ) const;
		
		Json::Value serialize( void ) const;
		bool unserialize( const Json::Value & );
		
		uint num;
	};
	
	class Axis : public Assignment
	{
	public:
		int analog( const Controller & ) const;
		
		Json::Value serialize( void ) const;
		bool unserialize( const Json::Value & );
		
		uint num;
		bool positive_direction;
	};
	
	class Hat : public Assignment
	{
	public:
		bool digital( const Controller & ) const;
		
		Json::Value serialize( void ) const;
		bool unserialize( const Json::Value & );
		
		uint num;
		bool y_axis, positive_direction;
	};
};

#endif // CONTROLLER_JOYSTICK_HPP
