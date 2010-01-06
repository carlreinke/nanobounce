#ifndef MENU_HPP
#define MENU_HPP

#include "loop.hpp"
#include "SDL.h"

class SimpleMenu : public Loop
{
public:
	SimpleMenu( bool no_fade = false );
	virtual ~SimpleMenu( void );
	
	void handle_event( SDL_Event & );
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
	std::vector<std::string> entries;
	uint selection;
	bool no_selection;
	
private:
	SDL_Surface *background;
};

class GameMenu : public Loop
{
public:
	GameMenu( void );
	
	void handle_event( SDL_Event & );
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
private:
	std::vector<std::string> entries;
	uint selection;
	
	Ball ball;
};

void pack_menu( SDL_Surface *surface );

#endif // MENU_HPP
