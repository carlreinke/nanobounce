#ifndef GAME_MENUS_HPP
#define GAME_MENUS_HPP

#include "level_set.hpp"
#include "menu.hpp"

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

#endif // GAME_MENUS_HPP
