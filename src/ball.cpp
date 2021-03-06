/*  nanobounce/src/ball.cpp
 *  
 *  Copyright 2015 Carl Reinke
 *  
 *  This program is non-commercial, open-source software; you can redistribute
 *  it and/or modify it under the terms of the MAME License as included along
 *  with this program.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  MAME License for more details.
 *  
 *  You should have received a copy of the MAME License along with this
 *  program; if not, see <http://www.intoorbit.org/licenses/MAME.txt>.
 */
#include "audio/audio.hpp"
#include "ball.hpp"
#include "block.hpp"
#include "level.hpp"
#include "misc.hpp"
#include "sdl_ext.hpp"

using std::max;
using std::min;

const int Ball::width = 2;
const int Ball::height = 2;

const Fixed Ball::constant = make_frac<Fixed>(5, 4);

// terminal velocity and acceleration
const Fixed Ball::y_term_vel = constant;
const Fixed Ball::y_accel    = sqr(-y_term_vel) / (2 * (1.5f * LevelBlock::height));

const Fixed Ball::x_term_vel   = (3.0f * LevelBlock::width) / ((2 * y_term_vel) / y_accel);
const Fixed Ball::push_x_accel = sqr(x_term_vel) / (2.0f * LevelBlock::width);

// boost block presets
const Fixed Ball::x_boost_block = constant;
const Fixed Ball::y_boost_block = sqrtf(2 * y_accel * ((4.0f * LevelBlock::height) - height));

// wall jump presets
const Fixed Ball::wall_jump_x_vel = constant;
const Fixed Ball::wall_jump_y_vel = wall_jump_x_vel * 0.5f;
const Fixed Ball::wall_jump_y_vel_reset = (-1.0f * LevelBlock::width * y_accel) / (2 * x_term_vel);
const int Ball::wall_jump_ticks = (2.0f * LevelBlock::width) / wall_jump_x_vel;

std::vector<Sprite> Ball::sprites;

Ball::Ball( Fixed x, Fixed y )
: x(x), y(y),
  no_vel(false), x_vel(0), y_vel(0),
  no_vel_limits(false),
  no_accel(false), x_accel(0),
  
  // unboost
  can_unboost(false),
  ticks_until_unboost(0),
  x_vel_unboost(0), y_vel_unboost(0),
  
  trail(trail_max)
{
	// build sprites
	if (sprites.empty())
	{
		for (uint i = 0; i < trail_max; ++i)
		{
			int temp = 255 - i * 255 / trail_max;
			sprites.emplace_back(width, height, SDL_Color_RGBA(temp, temp, temp));
		}
	}
}

void Ball::draw( SDL_Surface *surface, int x_offset, int y_offset, Uint8 alpha ) const
{
	for (auto coord = trail.begin(); coord != trail.end(); ++coord)
		sprites[trail.end() - coord - 1].blit(surface, x_offset + coord->x, y_offset + coord->y, alpha);
}

void Ball::tick( int x_push_direction )
{
	// keep track of push for wall jumping
	x_accel = x_push_direction * push_x_accel;
	
	if (!no_vel)
	{
		if (!no_accel)
		{
			x_vel += x_accel;
			y_vel += y_accel;
		}
		
		if (!no_vel_limits)
		{
			x_vel = min(max(-x_term_vel, x_vel), x_term_vel);
			y_vel = min(max(-y_boost_block, y_vel), y_term_vel);
		}
		
		x += x_vel;
		y += y_vel;
	}
	
	trail.push_back(Coord(x + make_frac<Fixed>(1, 2), y + make_frac<Fixed>(1, 2)));
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
	ticks_until_unboost = 0;
	
	// because ball gets relocated before boost
	trail.push_back(Coord(x, y));
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
	ticks_until_unboost = 0;
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
	ticks_until_unboost = wall_jump_ticks;
}
