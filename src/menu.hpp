#ifndef MENU_HPP
#define MENU_HPP

#include "level_set.hpp"
#include "loop.hpp"
#include "SDL.h"

class SimpleMenu : public Loop
{
public:
	SimpleMenu( SDL_Surface *background );
	~SimpleMenu( void );
	
	void handle_event( SDL_Event & );
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
	std::vector<std::string> entries;
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
	
	std::vector<const std::string *> entries;
	uint selection;
	bool no_selection;
	
protected:
	Fixed y, y_vel, y_accel;
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

class LevelSetMenu : public SmoothMenu
{
public:
	LevelSetMenu( bool allow_new = false );
	
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
	std::vector<LevelSet> entries;
};

class LevelMenu : public SmoothMenu
{
public:
	LevelMenu( const LevelSet &, bool allow_new = false );
};

#endif // MENU_HPP
