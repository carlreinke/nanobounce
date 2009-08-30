#include "audio.hpp"
#include "ball.hpp"
#include "block.hpp"
#include "level.hpp"
#include "misc.hpp"

using namespace std;

// terminal velocity and acceleration
const Fixed Ball::y_term_vel = 1.0f;
const Fixed Ball::y_accel    = sqr(-y_term_vel) / (2 * (1.5f * Block::height));

const Fixed Ball::x_term_vel   = (3.0f * Block::width) / ((2 * y_term_vel) / y_accel);
const Fixed Ball::push_x_accel = sqr(x_term_vel) / (2.0f * Block::width);

// boost block presets
const Fixed Ball::x_boost_block = 1.0f;
const Fixed Ball::y_boost_block = sqrtf(2 * y_accel * ((4.0f * Block::height) - height));

// wall jump presets
const Fixed Ball::wall_jump_x_vel = 1.0f;
const Fixed Ball::wall_jump_y_vel = wall_jump_x_vel * 0.5f;
const Fixed Ball::wall_jump_y_vel_reset = (-1.0f * Block::width * y_accel) / (2 * x_term_vel);
const int Ball::wall_jump_ticks = (2.0f * Block::width) / wall_jump_x_vel;

std::vector<Sprite> Ball::sprites;

Ball::Ball( Fixed x, Fixed y )
: x(x), y(y),
  no_vel(false), x_vel(0), y_vel(0),
  no_vel_limits(false),
  no_accel(false), x_accel(0),
  
  // unboost
  can_unboost(false), user_can_unboost(false),
  ticks_until_unboost(0),
  x_vel_unboost(0), y_vel_unboost(0)
{
	// build sprites
	if (sprites.empty())
	{
		for (unsigned int i = 0; i < trail_max; ++i)
		{
			int temp = 255 - i * (256 / trail_max);
			SDL_Color color = { temp, temp, temp };
			sprites.push_back(Sprite(width, height, color));
		}
	}
}

void Ball::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	for (deque<coord>::const_iterator i = trail.begin(); i != trail.end(); ++i)
		sprites[trail.end() - i - 1].blit(surface, i->first, i->second, alpha);
}

void Ball::update( int x_push_direction )
{
	// keep track of push for wall jumping
	x_accel = x_push_direction * push_x_accel;
	
	if (can_unboost)
	{
		if (user_can_unboost)
		{
			// if boost is not time-based, user can cancel the boost
			if ((was_pushed_left() && is_moving_right()) ||
			    (was_pushed_right() && is_moving_left()))
			{
				unboost();
				
				streams.push_back(Stream(samples["unboost"], 1, (Fixed)x / (Fixed)Level::width)); //! play_sample()
			}
		}
		else if (--ticks_until_unboost == 0)
			unboost();
	}
	
	if (!no_vel)
	{
		if (!no_accel)
		{
			x_vel += x_accel;
			y_vel += y_accel;
		}
		
		if (!no_vel_limits)
		{
			x_vel = max(-x_term_vel, min(x_vel, x_term_vel));
			y_vel = min(y_vel, y_term_vel); // no max up speed for y boost
		}
		
		x += x_vel;
		y += y_vel;
	}
	
	trail.push_back(coord(x, y));
	if (trail.size() > trail_max)
		trail.pop_front();
}

void Ball::x_boost( Fixed x_boost )
{
	no_vel_limits = true;
	no_accel = true;
	
	x_vel = x_boost;
	y_vel = 0;
	
	x_vel_unboost = x_vel;
	y_vel_unboost = y_vel;
	
	can_unboost = true;
	user_can_unboost = true;
	
	// because ball gets relocated before boost
	trail.push_back(coord(x, y));
	if (trail.size() > trail_max)
		trail.pop_front();
}

void Ball::y_boost( Fixed y_boost )
{
	if (can_unboost)
		unboost();
	
	y_vel = y_boost;
}

void Ball::unboost( void )
{
	no_vel_limits = false;
	no_accel = false;
	
	x_vel = x_vel_unboost;
	y_vel = y_vel_unboost;
	
	can_unboost = false;
	user_can_unboost = false;
}

void Ball::wall_jump( void )
{
	no_vel_limits = true;
	no_accel = true;
	
	x_vel = was_pushed_left() ? -wall_jump_x_vel : wall_jump_x_vel;
	y_vel = -wall_jump_y_vel;
	
	x_vel_unboost = x_vel;
	y_vel_unboost = wall_jump_y_vel_reset;
	
	can_unboost = true;
	user_can_unboost = false;
	ticks_until_unboost = wall_jump_ticks;
}
