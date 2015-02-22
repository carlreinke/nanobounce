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
	Game( const Level &, Controllers = controllers );
	
	void loop( SDL_Surface *surface );
	
protected:
	void handle_event( SDL_Event & );
	
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;

private:	
	void reset( void );
	
	void tick( void );
	
public:
	enum State
	{
		NONE,
		WON,
		CHEAT_WON,
		LOST,
		
		RESTART,
		QUIT
	};
	
	State state;
	
	Highscore highscore;
	
	struct Samples
	{
		Sample bounce;
		Sample unboost;
		Sample wall_jump;
		Sample boost;
		Sample nuke;
		Sample recycle;
		Sample toggle;
		Sample lost;
		Sample won;
	};

	static void load_resources( void );
	static Samples samples;
	
private:
	bool check_collide( Ball &, int = 0 );
	Fixed collision_depth_fraction( const Ball &, const GameBlock &, Fixed &revert_x, Fixed &revert_y, bool &hit_x, bool &hit_y, Fixed &edge_dist_x, Fixed &edge_dist_y ) const;
	void handle_noncollidable_block( Ball &, const GameBlock & );
	void handle_block_x_collision( Ball & );
	void handle_block_y_collision( Ball &, GameBlock & );
	
	void check_unboost( Ball & );
	
	static bool ball_overlaps_rect( const Ball &, int x, int y, int w, int h );
	static bool ball_overlaps_block( const Ball &, const GameBlock & );
	static bool ball_half_inside_block( const Ball &ball, const GameBlock &block );
	bool ball_overlaps_level( const Ball & ) const;
	bool ball_overlaps_any_block( const Ball & ) const;
	
	Fixed sample_pan( const Fixed &x ) const { return (x_offset + x) / screen_width; }
	
	int x_offset, y_offset;
	
	sha256 level_hash;
	
	const int width, height;
	
	std::vector<GameBlock> blocks;
	
	std::vector<Ball> balls;
	
	std::list<Particle> particles;
	
	void menu( void );
	
	Controllers play_controllers;
};

#endif // GAME_HPP
