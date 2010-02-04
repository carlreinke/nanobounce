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
 *  program; if not, see <http://www.nothinglost.net/licenses/MAME.txt>.
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
	     << SDL_JoystickNumButtons(joystick) << " buttons)" << endl;
}

Joystick::~Joystick( void )
{
	if (joystick != NULL)
	{
		SDL_JoystickClose(joystick);
		joystick = NULL;
	}
}

void Joystick::update_down( void )
{
	if (joystick == NULL)
		return;
	
#ifdef TARGET_GP2X
	is_down[left]  = SDL_JoystickGetButton(joystick, GP2X_VK_UP_LEFT) ||
	                 SDL_JoystickGetButton(joystick, GP2X_VK_LEFT) ||
	                 SDL_JoystickGetButton(joystick, GP2X_VK_DOWN_LEFT);
	is_down[right] = SDL_JoystickGetButton(joystick, GP2X_VK_UP_RIGHT) ||
	                 SDL_JoystickGetButton(joystick, GP2X_VK_RIGHT) ||
	                 SDL_JoystickGetButton(joystick, GP2X_VK_DOWN_RIGHT);
	
	is_down[left_shoulder]  = SDL_JoystickGetButton(joystick, GP2X_VK_FL);
	is_down[right_shoulder] = SDL_JoystickGetButton(joystick, GP2X_VK_FR);
	
	is_down[up]   = SDL_JoystickGetButton(joystick, GP2X_VK_UP_LEFT) ||
	                SDL_JoystickGetButton(joystick, GP2X_VK_UP) ||
	                SDL_JoystickGetButton(joystick, GP2X_VK_UP_RIGHT);
	is_down[down] = SDL_JoystickGetButton(joystick, GP2X_VK_DOWN_LEFT) ||
	                SDL_JoystickGetButton(joystick, GP2X_VK_DOWN) ||
	                SDL_JoystickGetButton(joystick, GP2X_VK_DOWN_RIGHT);
	
	is_down[select] = SDL_JoystickGetButton(joystick, GP2X_VK_FB);
	is_down[back]   = SDL_JoystickGetButton(joystick, GP2X_VK_FX);
	
	is_down[start] = SDL_JoystickGetButton(joystick, GP2X_VK_START);
	is_down[quit]  = SDL_JoystickGetButton(joystick, GP2X_VK_START) &&
	                 SDL_JoystickGetButton(joystick, GP2X_VK_SELECT);
	
	is_down[vol_up]   = SDL_JoystickGetButton(joystick, GP2X_VK_VOL_UP);
	is_down[vol_down] = SDL_JoystickGetButton(joystick, GP2X_VK_VOL_DOWN);
#else
	is_down[left]  = SDL_JoystickGetAxis(joystick, 0) < numeric_limits<Sint16>::min() / 2;
	is_down[right] = SDL_JoystickGetAxis(joystick, 0) > numeric_limits<Sint16>::max() / 2;
	
	is_down[up]   = SDL_JoystickGetAxis(joystick, 1) < numeric_limits<Sint16>::min() / 2;
	is_down[down] = SDL_JoystickGetAxis(joystick, 1) > numeric_limits<Sint16>::max() / 2;
	
	is_down[select] = SDL_JoystickGetButton(joystick, 0);
	is_down[back]   = SDL_JoystickGetButton(joystick, 1);
	
	is_down[start] = SDL_JoystickGetButton(joystick, 2);
	is_down[quit]  = SDL_JoystickGetButton(joystick, 3);
#endif
}
