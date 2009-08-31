#ifndef GAME_HPP
#define GAME_HPP

#include "highscore.hpp"
#include "level.hpp"
#include "SDL.h"

class Game
{
public:
	Game( void );
	
	bool load( const std::string &level_data_path );
	void reset( void );
	
	void tick( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
	enum State
	{
		none,
		quit,
		won,
		lost
	};
	State state;
	
private:
	void check_collide( Ball &, Block & );
	
	int x_offset, y_offset;
	
	Level level;
	std::vector<Ball> balls;
	
	friend void play_pack( SDL_Surface *, const std::string & ); // TODO fix this
};

void play_pack( SDL_Surface *surface, const std::string &directory );
void pack_done_screen( SDL_Surface *surface, const std::string &pack_name );

void level_screen( SDL_Surface *surface, const Level &level, const Highscore &highscore );
void level_loop( SDL_Surface *surface, Game &, Highscore &new_highscore );

#endif // GAME_HPP
