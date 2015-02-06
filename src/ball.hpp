#ifndef BALL_HPP
#define BALL_HPP

#include "fixed.hpp"
#include "video/sprite.hpp"
#include "SDL.h"

class Ball
{
public:
	Ball( Fixed x, Fixed y );
	
	static void static_destruction_clean_up( void ) { sprites.clear(); }
	
	void draw( SDL_Surface *, int x_offset = 0, int y_offset = 0, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	void tick( int x_push_direction );
	
private:
	Fixed x, y;
	static const int height, width;
	
	static const Fixed constant;  // base value for calculation of motion constants
	
	// velocity
	bool no_vel;
	Fixed x_vel, y_vel;
	
	// terminal velocity
	bool no_vel_limits;
	static const Fixed y_term_vel, x_term_vel;
	
	// acceleration
	bool no_accel;
	static const Fixed y_accel, push_x_accel;
	Fixed x_accel;
	
	// after unboost
	bool can_unboost;
	int ticks_until_unboost;
	Fixed x_vel_unboost, y_vel_unboost;
	
	// boost presets: boost blocks
	static const Fixed x_boost_block, y_boost_block;
	
	// boost presets: wall jump
	static const Fixed wall_jump_x_vel, wall_jump_y_vel;
	static const Fixed wall_jump_y_vel_reset;
	static const int wall_jump_ticks;
	
	struct Coord
	{
		Coord( int x, int y ) : x(x), y(y) { }
		
		int x;
		int y;
	};
	
	static const uint trail_max = 40;
	std::deque<Coord> trail;
	
	static std::vector<Sprite> sprites;
	
	void x_boost( Fixed x_boost );
	void y_boost( Fixed y_boost );
	void unboost( void );
	void wall_jump( void );
	
	bool is_moving_left( void ) const { return (x_vel < 0); }
	bool is_moving_right( void ) const { return (x_vel > 0); }
	bool is_moving_up( void ) const { return (y_vel < 0); }
	bool is_moving_down( void ) const { return (y_vel > 0); }
	
	bool was_pushed_left( void ) const { return (x_accel < 0); }
	bool was_pushed_right( void ) const { return (x_accel > 0); }
	
	friend class Game;
	friend class GameMenu;
};

#endif // BALL_HPP
