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
 *  program; if not, see <http://www.nothinglost.net/licenses/MAME.txt>.
 */
#ifndef CONTROLLER_KEYBOARD_HPP
#define CONTROLLER_KEYBOARD_HPP

#include "controller/controller.hpp"

class Keyboard : public ConfigurableController
{
public:
	Keyboard( void );
	
	bool is_keyboard( void ) { return true; }
	
private:
	Uint8 *key_state;
	
	const Json::Value &assignment_root( const Json::Value & ) const;
	boost::shared_ptr<Assignment> parse_assignment( const Json::Value & ) const;
	
	class Key : public Assignment
	{
	public:
		bool digital( const Controller & ) const;
		
		Json::Value serialize( void ) const;
		bool unserialize( const Json::Value & );
		
		uint num;
	};
};

#endif // CONTROLLER_KEYBOARD_HPP
