#ifndef GAME_HPP
#define GAME_HPP

#include "highscore.hpp"
#include "level.hpp"
#include "loop.hpp"
#include "particle.hpp"
#include "sdl_ext.hpp"

class Game : public Loop
{
public:
	Game( void );
	Game( const Level & );
	
	void handle_event( SDL_Event & );
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
	bool load( const std::string &level_data_path );
	void reset( void );
	
	void tick( void );
	
	enum State
	{
		none,
		quit,
		won,
		cheat_won,
		lost
	};
	State state;
	
	Highscore highscore;
	
private:
	void check_unboost( Ball & );
	
	void check_collide( Ball &, Block & );
	bool is_outside( const Ball &, const Level & ) const;
	bool is_conflicted( const Ball &, const Level & ) const;
	
	Fixed sample_pan( const Fixed &x ) const { return (x_offset + x) / screen_width; }
	
	int x_offset, y_offset;
	
	Level level;
	std::vector<Ball> balls;
	
	std::list<Particle> particles;
	
	void menu( void );
};

#endif // GAME_HPP
