/*  nanobounce/src/menu.hpp
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
#ifndef MENU_HPP
#define MENU_HPP

#include "loop.hpp"

#include <SDL.h>

class SimpleMenu : public Loop
{
public:
	SimpleMenu( SDL_Surface *background = NULL );
	~SimpleMenu( void );
	
protected:
	void handle_event( SDL_Event & );
	
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
public:
	std::vector<std::string> entries;
	virtual uint entry_count( void ) const;
	
	uint selection;
	bool no_selection;
	
private:
	SDL_Surface *background;
};

class SmoothMenu : public Loop
{
public:
	SmoothMenu( void );
	
protected:
	void handle_event( SDL_Event & );
	
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
public:
	std::vector<std::string> entries;
	virtual uint entry_count( void ) const;
	
	uint selection;
	bool no_selection;
	
protected:
	Fixed y, y_vel, y_accel;
	
	uint entry_height;
};

#endif // MENU_HPP
