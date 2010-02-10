#ifndef GAME_HPP
#define GAME_HPP

#include "audio/sample.hpp"
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
	
	static void load_resources( void );
	static struct Samples
	{
		Sample bounce, unboost, wall_jump,
		       boost, nuke, recycle, toggle,
		       lost, won;
	}
	samples;
	
private:
	void check_unboost( Ball & );
	
	void check_collide( Ball &, Block & );
	
	static bool ball_inside_rect( const Ball &, int x, int y, int w, int h );
	static bool ball_inside_block( const Ball &, const Block & );
	bool ball_inside_level( const Ball & ) const;
	bool ball_inside_any_block( const Ball & ) const;
	
	Fixed sample_pan( const Fixed &x ) const { return (x_offset + x) / screen_width; }
	
	int x_offset, y_offset;
	
	Level level;
	std::vector<Ball> balls;
	
	std::list<Particle> particles;
	
	void menu( void );
};

#endif // GAME_HPP
