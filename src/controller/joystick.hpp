/*  controller.hpp
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
	
	const Json::Value &get_config( const Json::Value & ) const;
	std::unique_ptr<Input> parse_input( const Json::Value & ) const;
	
	class Button : public Input
	{
	public:
		bool digital( const Controller & ) const;
		
		Json::Value serialize( void ) const;
		bool unserialize( const Json::Value & );
		
		uint num;
	};
	
	class Axis : public Input
	{
	public:
		int analog( const Controller & ) const;
		
		Json::Value serialize( void ) const;
		bool unserialize( const Json::Value & );
		
		uint num;
		bool positive_direction;
	};
	
	class Hat : public Input
	{
	public:
		bool digital( const Controller & ) const;
		
		Json::Value serialize( void ) const;
		bool unserialize( const Json::Value & );
		
		uint num;
		bool y_axis;
		bool positive_direction;
	};
};

#endif // CONTROLLER_JOYSTICK_HPP
