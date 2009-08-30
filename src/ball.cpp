#include "audio.hpp"
#include "ball.hpp"
#include "block.hpp"
#include "level.hpp"

using namespace std;

std::vector<Sprite> Ball::sprites;

Ball::Ball( Fixed x, Fixed y )
: x(x), y(y), x_vel(0), y_vel(0),
  x_boost(0), y_boost(0), boost_ticks_left(0), is_wall_jumping(false)
{
	// terminal velocities
	y_term_vel = 1.0f;
	Fixed gravity = (-y_term_vel * -y_term_vel) / (2 * (1.5f * Block::height));
	x_term_vel = (3.0f * Block::width) / ((2 * y_term_vel) / gravity);
	
	// accelerations
	x_accel = 0;
	y_accel = gravity;
	x_push_accel = (x_term_vel * x_term_vel) / (2.0f * Block::width);
	
	// boost blocks
	x_boost_block = 1.0f;
	y_boost_block = sqrtf(2 * gravity * ((4.0f * Block::height) - height));
	
	// wall jump
	x_wall_jump = 1.0f;
	y_wall_jump = x_wall_jump * 0.5f;
	wall_jump_ticks = (2.0f * Block::width) / x_wall_jump;
	y_wall_jump_reset = (-1.0f * Block::width * gravity) / (2 * x_term_vel);
	
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
	x_accel = x_push_direction * x_push_accel;
	
	if (boost_ticks_left != 0)
	{
		--boost_ticks_left;
		
		x_vel = x_boost;
		y_vel = y_boost;
		
		if (is_wall_jumping) // wall jumps can't be stopped
		{
			if (boost_ticks_left == 0)
			{
				y_vel = y_wall_jump_reset;
				
				is_wall_jumping = false;
			}
		}
		else if ((was_pushed_left() && is_moving_right()) ||
		         (was_pushed_right() && is_moving_left()))
		{
			stop_boost();
			
			streams.push_back(Stream(samples["unboost"], 1, (Fixed)x / (Fixed)Level::width)); //! play_sample()
		}
	}
	else
	{
		x_vel += x_accel;
		y_vel += y_accel;
		
		x_vel = max(-x_term_vel, min(x_vel, x_term_vel));
		y_vel = min(y_vel, y_term_vel); // no max up speed for y boost
	}
	
	x += x_vel;
	y += y_vel;
	
	trail.push_back(coord(x, y));
	if (trail.size() > trail_max)
		trail.pop_front();
}

void Ball::boost( Fixed x, Fixed y, int ticks, bool is_wall_jump )
{
	if (x == 0)
	{
		y_vel = y;
		return;
	}
	x_vel = (x < 0) ? -fabs(x_vel) : fabs(x_vel);
	y_vel = (y < 0) ? -fabs(y_vel) : fabs(y_vel);
	
	x_boost = x;
	y_boost = y;
	boost_ticks_left = ticks;
	
	is_wall_jumping = is_wall_jump;
}

void Ball::stop_boost( void )
{
	boost_ticks_left = 0;
	
	is_wall_jumping = false;
}

void Ball::wall_jump( void )
{
	boost(was_pushed_left() ? -x_wall_jump : x_wall_jump, -y_wall_jump, wall_jump_ticks, true);
}
