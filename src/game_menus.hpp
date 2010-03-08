#ifndef GAME_MENUS_HPP
#define GAME_MENUS_HPP

#include "level_set.hpp"
#include "menu.hpp"

class GameMenu : public SimpleMenu
{
public:
	GameMenu( void );
	
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
private:
	Ball ball;
};

class LevelSetMenu : public SmoothMenu
{
public:
	LevelSetMenu( bool allow_new = false );
	
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
	std::vector<LevelSet> entries;
	uint entry_count( void ) const;
};

class LevelMenu : public SmoothMenu
{
public:
	LevelMenu( const LevelSet &, bool allow_new = false );
};

class ScoredLevelMenu : public SmoothMenu
{
public:
	ScoredLevelMenu( const LevelSet &, bool show_one_incomplete = true, bool auto_select_single_entry = true );
	
	void loop( SDL_Surface * );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
	struct Entry { std::string name, path, score_name, score_time; };
	std::vector<Entry> entries;
	uint entry_count( void ) const;
	
private:
	bool auto_select_single_entry;
};

class TextEntryMenu : public SmoothMenu
{
public:
	TextEntryMenu( const std::string &title, const std::string &text );
	
	void handle_event( SDL_Event & );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
	std::string text;
	
private:
	std::string title;
};

#endif // GAME_MENUS_HPP
