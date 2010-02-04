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
 *  program; if not, see <http://www.nothinglost.net/licenses/MAME.txt>.
 */
#include "controller/keyboard.hpp"

using namespace std;

Keyboard::Keyboard( void )
{
	key_state = SDL_GetKeyState(NULL);
}

void Keyboard::update_down( void )
{
	is_down[left]  = key_state[left_key];
	is_down[right] = key_state[right_key];
	
	is_down[left_shoulder]  = key_state[left_shoulder_key];
	is_down[right_shoulder] = key_state[right_shoulder_key];
	
	is_down[up]   = key_state[up_key];
	is_down[down] = key_state[down_key];
	
	is_down[select] = key_state[select_key];
	is_down[back]   = key_state[back_key];
	
	is_down[start] = key_state[start_key];
	is_down[quit]  = key_state[quit_key];
	
	is_down[vol_up]   = key_state[vol_up_key] || key_state[SDLK_EQUALS];
	is_down[vol_down] = key_state[vol_down_key] || key_state[SDLK_UNDERSCORE];
}
