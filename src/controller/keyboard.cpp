/*  controller.cpp
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
#include "controller/keyboard.hpp"

using namespace std;

Keyboard::Keyboard( void )
{
	key_state = SDL_GetKeyState(NULL);
	
	load_assignments();
}

const Json::Value &Keyboard::assignment_root( const Json::Value &root ) const
{
	return root["keyboard"];
}

boost::shared_ptr<Keyboard::Assignment> Keyboard::parse_assignment( const Json::Value &serialized ) const
{
	boost::shared_ptr<Assignment> temp;
	
	if (serialized.isMember("key"))
	{
		temp = boost::shared_ptr<Assignment>(new Key);
		temp->unserialize(serialized);
	}
	
	return temp;
}


bool Keyboard::Key::digital( const Controller &controller ) const
{
	return dynamic_cast<const Keyboard &>(controller).key_state[num];
}

Json::Value Keyboard::Key::serialize( void ) const
{
	Json::Value root;
	
	root["key"] = num;
	
	return root;
}

bool Keyboard::Key::unserialize( const Json::Value &serialized )
{
	num = serialized.get("key", 0).asInt();
	
	return true;
}
