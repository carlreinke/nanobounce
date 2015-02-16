#ifndef GAME_MENUS_HPP
#define GAME_MENUS_HPP

#include "level_pack.hpp"
#include "menu.hpp"
#include "highscore.hpp"

class GameMenu : public SimpleMenu
{
public:
	GameMenu( void );
	
protected:
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
private:
	Ball ball;
};

class LevelPackMenu : public SmoothMenu
{
public:
	LevelPackMenu( bool allow_new = false );
	
protected:
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
public:
	struct Entry
	{
		bool no_sort;
		LevelPack level_pack;
		
		bool operator<( const Entry &that ) const { return no_sort ? false :
		        level_pack.get_name() != that.level_pack.get_name() ? level_pack.get_name() < that.level_pack.get_name() :
		                                                              level_pack.get_author() < that.level_pack.get_author(); }
	};
	
	std::vector<Entry> entries;
	uint entry_count( void ) const;
};

class LevelMenu : public SmoothMenu
{
public:
	LevelMenu( const LevelPack &, bool allow_new = false );
};

class ScoredLevelMenu : public SmoothMenu
{
public:
	ScoredLevelMenu( const LevelPack &, bool show_one_incomplete = true, bool auto_select_single_entry = true );
	
	void loop( SDL_Surface * );
	
protected:
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
public:
	struct Entry
	{
		std::string level_name;
		std::string score_name;
		std::string score_time;
	};
	
	std::vector<Entry> entries;
	uint entry_count( void ) const;
	
private:
	bool auto_select_single_entry;
};

class TextEntryMenu : public SmoothMenu
{
public:
	TextEntryMenu( const std::string &title, const std::string &text );
	
protected:
	void handle_event( SDL_Event & );
	
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
public:
	std::string text;
	
private:
	std::string title;
};

#endif // GAME_MENUS_HPP
