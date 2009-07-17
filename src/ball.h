#ifndef BALL_H
#define BALL_H

#include "fixed.h"
#include "sprite.h"
#include "SDL.h"

class Ball
{
public:
	Ball( Fixed x, Fixed y );
	
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	void update( int x_push );
	
private:
	Fixed x, y;
	static const int height = 2, width = 2;
	
	Fixed x_vel, y_vel;
	Fixed x_term_vel, y_term_vel;
	
	Fixed x_boost, y_boost;
	int boost_ticks_left;
	bool is_wall_jumping;
	
	// block boost presets
	Fixed x_boost_block, y_boost_block;
	
	// wall jump boost presets
	Fixed x_wall_jump, y_wall_jump, y_wall_jump_reset;
	int wall_jump_ticks;
	
	Fixed x_accel, y_accel, x_push_accel;
	
	typedef std::pair<int, int> coord;
	
	static const unsigned int trail_max = 50;
	std::deque<coord> trail;
	
	static std::vector<Sprite> sprites;
	
	void boost( Fixed x, Fixed y, int ticks = -1, bool is_wall_jump = false );
	void stop_boost();
	void wall_jump( void );
	
	bool is_moving_left( void ) const;
	bool is_moving_right( void ) const;
	bool is_moving_up( void ) const;
	bool is_moving_down( void ) const;
	
	bool was_pushed_left( void ) const;
	bool was_pushed_right( void ) const;
	
	friend class Level;
	friend void game_menu( SDL_Surface * );
};

inline bool Ball::is_moving_left( void ) const
{
	return (x_vel < 0);
}

inline bool Ball::is_moving_right( void ) const
{
	return (x_vel > 0);
}

inline bool Ball::is_moving_up( void ) const
{
	return (y_vel < 0);
}

inline bool Ball::is_moving_down( void ) const
{
	return (y_vel > 0);
}

inline bool Ball::was_pushed_left( void ) const
{
	return (x_accel < 0);
}

inline bool Ball::was_pushed_right( void ) const
{
	return (x_accel > 0);
}

#endif
