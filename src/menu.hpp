#ifndef MENU_HPP
#define MENU_HPP

#include "loop.hpp"
#include "SDL.h"

class SimpleMenu : public Loop
{
public:
	SimpleMenu( SDL_Surface *background = NULL );
	~SimpleMenu( void );
	
	void handle_event( SDL_Event & );
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
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
	
	void handle_event( SDL_Event & );
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
	std::vector<std::string> entries;
	virtual uint entry_count( void ) const;
	
	uint selection;
	bool no_selection;
	
protected:
	Fixed y, y_vel, y_accel;
};

#endif // MENU_HPP
