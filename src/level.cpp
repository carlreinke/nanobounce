#include "audio.h"
#include "controller.h"
#include "level.h"
#include "misc.h"
#include "SDL.h"

using namespace std;

Level::Level( void )
: done(false), state(none), fade(0)
{
	// good to go
}

Level::Level( istream &data )
{
	load(data);
}

void Level::load( istream &data )
{
	done = false;
	state = none;
	
	balls.clear();
	blocks.clear();
	
	getline(data, name);
	
	for (int y = 0; y < height && !data.fail(); y += Block::height)
	{
		istringstream line;
		getline(data, line);
		
		for (int x = 0; x < width; x += Block::width)
		{
			char type;
			line.get(type);
			line.ignore(1);
			
			if ((int)type == (int)Block::ball)
				balls.push_back(Ball(x + Block::width / 2, y + Block::height / 2)); 
			
			if ((int)type != (int)Block::none)
				blocks.push_back(Block(x, y, (Block::types)type));
		}
	}
	
	if (data.fail())
	{
		cout << "warning: level failed to load" << endl;
		state = load_failed;
	}
	else
		cout << "loaded level '" << name << "'" << endl;
}

void Level::draw( SDL_Surface *surface ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	for (vector<Block>::const_iterator block = blocks.begin(); block != blocks.end(); ++block)
		block->draw(surface, fade);
	
	for (vector<Ball>::const_iterator ball = balls.begin(); ball != balls.end(); ++ball)
		ball->draw(surface, fade);
}

void Level::update( int ball_x_direction )
{
	for (vector<Ball>::iterator ball = balls.begin(); ball != balls.end(); ++ball)
	{
		ball->update(ball_x_direction);
		
		for (vector<Block>::iterator block = blocks.begin(); block != blocks.end(); ++block)
			check_collide(*ball, *block);
		
		if (state == none)
		{
			if ((int)ball->x + Ball::width <= 0 ||
			    (int)ball->x >= Level::width ||
			    (int)ball->y + Ball::height <= 0 ||
			    (int)ball->y >= Level::height)
			{
				state = lost;
				
				streams.push_back(Stream(samples["lost"], 1, (Fixed)ball->x / (Fixed)Level::width)); //! play_sample()
			}
		}
	}
	
	if (state == none && fade < SDL_ALPHA_OPAQUE)
		fade += 5;
	else if (state != none && fade > SDL_ALPHA_TRANSPARENT)
		fade -= 5;
	else if (state != none && fade <= SDL_ALPHA_TRANSPARENT)
		done = true;
}

void Level::check_collide( Ball &ball, Block &block )
{
redo:
	bool x_in = (int)ball.x + ball.width > block.x &&
	            (int)ball.x < block.x + block.width;
	bool y_in = (int)ball.y + ball.height > block.y &&
	            (int)ball.y < block.y + block.height;
	
	if (x_in && y_in)
	{
		if (block.can_collide())
		{
			// keep ball outside blocks
			if (fabs(ball.y_vel) > 1)
			{
				ball.y -= ball.is_moving_up() ? -1 : 1;
				ball.trail.back().second = ball.y;
			}
			else
			{
				ball.x -= ball.is_moving_left() ? -1 : 1;
				ball.trail.back().first = ball.x;
			}
			
			goto redo;
		}
		else if (block.type == Block::exit && state != won)
		{
			state = won;
			
			streams.push_back(Stream(samples["won"], 1, (Fixed)ball.x / (Fixed)Level::width)); //! play_sample()
		}
	}
	
	if (!block.can_collide())
		return;
	
	Sample *sample = NULL;
	
	bool hit_top = false;
	
	if (x_in)
	{
		hit_top         = ball.is_moving_down() && (int)ball.y + ball.height == block.y;
		bool hit_bottom = ball.is_moving_up() && (int)ball.y == block.y + block.height;
		
		if (hit_top || hit_bottom)
		{
			ball.y_vel = hit_top ? -ball.y_term_vel : -ball.y_vel / 3;
			ball.stop_boost();
			
			sample = &samples["bounce"];
		}
	}
	
	if (y_in)
	{
		bool hit_left  = ball.is_moving_right() && (int)ball.x + ball.width == block.x;
		bool hit_right = ball.is_moving_left() && (int)ball.x == block.x + block.width;
		
		if (hit_left || hit_right)
		{
			ball.x_vel = -ball.x_vel / 3;
			ball.stop_boost();
			
			// wall jump
			if ((hit_left && ball.was_pushed_left()) ||
			    (hit_right && ball.was_pushed_right()))
			{
				ball.wall_jump();
				
				sample = &samples["wall_jump"];
			}
			else if (fabs(ball.x_vel) > 3 * ball.x_push_accel)
			{
				sample = &samples["bounce"];
			}
		}
	}
	
	if (hit_top)
	{
		switch (block.type)
		{
		case Block::bomb:
			ball.x_vel = ball.y_vel = 0;
			ball.x_push_accel = ball.y_accel = 0;
			
			//! explode block and/or ball into particles
			
			state = lost;
			
			sample = &samples["explode"];
			break;
			
		case Block::cracked:
			block.type = Block::none;
			
			//! explode block into particles
			
			sample = &samples["recycle"];
			break;
			
		case Block::boost_up:
			ball.boost(0, -ball.y_boost_block);
			
			sample = &samples["boost"];
			break;
			
		case Block::boost_left:
			ball.x = block.x - Ball::width + ball.x_boost_block;
			ball.y += Ball::height;
			ball.boost(-ball.x_boost_block, 0);
			
			sample = &samples["boost"];
			break;
			
		case Block::boost_right:
			ball.x = block.x + Block::width - ball.x_boost_block;
			ball.y += Ball::height;
			ball.boost(ball.x_boost_block, 0);
			
			sample = &samples["boost"];
			break;
			
		default:
			break;
		}
	}
	
	if (sample != NULL)
		streams.push_back(Stream(*sample, 1, (Fixed)ball.x / (Fixed)Level::width)); //! play_sample()
}
