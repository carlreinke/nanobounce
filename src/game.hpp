#ifndef GAME_HPP
#define GAME_HPP

#include "audio/sample.hpp"
#include "controller/controller.hpp"
#include "highscore.hpp"
#include "level.hpp"
#include "loop.hpp"
#include "particle.hpp"
#include "sdl_ext.hpp"

class Game : public Loop
{
public:
	Game( Controller::Set = controllers );
	Game( const Level &, Controller::Set = controllers );
	
	void handle_event( SDL_Event & );
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
	bool load( const std::string &level_data_path );
	void reset( void );
	
	void tick( void );
	
	enum State
	{
		none,
		won,
		cheat_won,
		lost,
		
		restart,
		quit
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
	bool check_collide( Ball &, int = 0 );
	Fixed collision_depth_fraction( const Ball &, const Block &, Fixed &revert_x, Fixed &revert_y, bool &hit_x, bool &hit_y, Fixed &edge_dist_x, Fixed &edge_dist_y ) const;
	void handle_noncollidable_block( Ball &, Block & );
	void handle_block_x_collision( Ball & );
	void handle_block_y_collision( Ball &, Block & );
	
	void check_unboost( Ball & );
	
	static bool ball_overlaps_rect( const Ball &, int x, int y, int w, int h );
	static bool ball_overlaps_block( const Ball &, const Block & );
	static bool ball_half_inside_block( const Ball &ball, const Block &block );
	bool ball_overlaps_level( const Ball & ) const;
	bool ball_overlaps_any_block( const Ball & ) const;
	
	Fixed sample_pan( const Fixed &x ) const { return (x_offset + x) / screen_width; }
	
	int x_offset, y_offset;
	
	Level level;
	std::vector<Ball> balls;
	
	std::list<Particle> particles;
	
	void menu( void );
	
	Controller::Set play_controllers;
	
public:
	static bool play( SDL_Surface *surface, std::pair< std::vector<Level>::iterator, std::vector<Level>::iterator > levels );
};

#endif // GAME_HPP
