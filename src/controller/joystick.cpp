/*  controller/joystick.cpp
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
#include "controller/joystick.hpp"

using namespace std;

Joystick::Joystick( int j )
: joystick(NULL)
{
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	
	if (j >= SDL_NumJoysticks())
	{
		cerr << "error: joystick " << j << " does not exist" << endl;
		return;
	}
	
	joystick = SDL_JoystickOpen(j);
	
	if (joystick == NULL)
	{
		cerr << "error: joystick " << j << " failed: " << SDL_GetError() << endl;
		return;
	}
	
	cout << "joystick '" << SDL_JoystickName(j) << "' ("
	     << SDL_JoystickNumAxes(joystick) << " axes, "
	     << SDL_JoystickNumButtons(joystick) << " buttons, "
	     << SDL_JoystickNumHats(joystick) << " hats)" << endl;
	
	load_assignments();
}

Joystick::~Joystick( void )
{
	if (joystick != NULL)
	{
		SDL_JoystickClose(joystick);
		joystick = NULL;
	}
}

const Json::Value &Joystick::assignment_root( const Json::Value &root ) const
{
	const char *name = SDL_JoystickName(SDL_JoystickIndex(joystick));
	
	return root["joystick"][root["joystick"].isMember(name) ? name : "defaults"];
}

std::shared_ptr<Joystick::Assignment> Joystick::parse_assignment( const Json::Value &serialized ) const
{
	std::shared_ptr<Assignment> temp;
	
	if (serialized.isMember("button"))
	{
		temp = std::make_shared<Button>();
		temp->unserialize(serialized);
	}
	else if (serialized.isMember("axis"))
	{
		temp = std::make_shared<Axis>();
		temp->unserialize(serialized);
	}
	else if (serialized.isMember("hat"))
	{
		temp = std::make_shared<Hat>();
		temp->unserialize(serialized);
	}
	
	return temp;
}


bool Joystick::Button::digital( const Controller &controller ) const
{
	return SDL_JoystickGetButton(dynamic_cast<const Joystick &>(controller).joystick, num);
}

Json::Value Joystick::Button::serialize( void ) const
{
	Json::Value root;
	
	root["button"] = num;
	
	return root;
}

bool Joystick::Button::unserialize( const Json::Value &serialized )
{
	num = serialized.get("button", 0).asInt();
	
	return true;
}

int Joystick::Axis::analog( const Controller &controller ) const
{
	Sint16 temp = SDL_JoystickGetAxis(dynamic_cast<const Joystick &>(controller).joystick, num);
	if (!positive_direction)
		temp = -temp - 1;
	return max(Sint16(0), temp);
}

Json::Value Joystick::Axis::serialize( void ) const
{
	Json::Value root;
	
	root["axis"] = num;
	root["direction"] = positive_direction ? "positive" : "negative";
	
	return root;
}

bool Joystick::Axis::unserialize( const Json::Value &serialized )
{
	num = serialized.get("axis", 0).asInt();
	positive_direction = (serialized.get("direction", "").asString() == "positive");
	
	return true;
}

bool Joystick::Hat::digital( const Controller &controller ) const
{
	Uint8 temp = SDL_JoystickGetHat(dynamic_cast<const Joystick &>(controller).joystick, num);
	
	temp &= y_axis ? (SDL_HAT_UP | SDL_HAT_DOWN) : (SDL_HAT_LEFT | SDL_HAT_RIGHT);
	temp &= positive_direction ? (SDL_HAT_RIGHT | SDL_HAT_DOWN) : (SDL_HAT_LEFT | SDL_HAT_UP);
	
	return temp;
}

Json::Value Joystick::Hat::serialize( void ) const
{
	Json::Value root;
	
	root["hat"] = num;
	root["hat_axis"] = y_axis ? "y" : "x";
	root["direction"] = positive_direction ? "positive" : "negative";
	
	return root;
}

bool Joystick::Hat::unserialize( const Json::Value &serialized )
{
	num = serialized.get("hat", 0).asInt();
	y_axis = (serialized.get("hat_axis", "").asString() == "y");
	positive_direction = (serialized.get("direction", "").asString() == "positive");
	
	return true;
}
