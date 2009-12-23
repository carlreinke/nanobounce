#include "audio.hpp"
#include "game.hpp"
#include "main.hpp"
#include "menu.hpp"
#include "misc.hpp"

using namespace std;

Game::Game( void )
: state(none)
{
	// good to go
}

Game::Game( const Level &level )
{
	this->level = level;
	reset();
}

void Game::handle_event( SDL_Event &e )
{
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case Controller::start_key:
			menu();
			break;
		case Controller::quit_key:
			state = quit;
			break;
		
		default:
			break;
		}
		break;
		
	default:
		break;
	}
}

void Game::update( void )
{
	// update controller
	for (vector<Controller *>::iterator c = controllers.begin(); c != controllers.end(); ++c)
		(*c)->update();
	
	if (!fader.is_fading(Fader::in))
	{
		for (int i = 0; i < 4; ++i)
		{
			// update replay controllers
			for (vector<Controller *>::iterator c = controllers.begin(); c != controllers.end(); ++c)
				(*c)->tick_update();
			
			tick();
		}
	}
	
	if (state != none)
		loop_quit = true;
}

void Game::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	level.draw(surface, x_offset, y_offset, alpha);
	
	for (vector<Ball>::const_iterator ball = balls.begin(); ball != balls.end(); ++ball)
		ball->draw(surface, x_offset, y_offset, alpha);
}

bool Game::load( const string &level_data_path )
{
	bool temp = level.load(level_data_path);
	reset();
	
	return temp;
}

void Game::reset( void )
{
	level.reset();
	highscore.reset(ms_per_update);
	
	balls.clear();
	for (vector<Block>::const_iterator b = level.blocks.begin(); b != level.blocks.end(); ++b)
	{
		if (b->type == Block::ball)
		{
			balls.push_back(Ball(b->x + Block::width / 2 - Ball::width / 2,
			                     b->y + Block::height / 2 - Ball::height / 2));
		}
	}
	
	state = none;
	x_offset = y_offset = 0;
}

void Game::tick( void )
{
	int x_direction = 0;
	
	for (vector<Controller *>::iterator c = controllers.begin(); c != controllers.end(); ++c)
	{
		const bool left = (*c)->is_down[Controller::left] || (*c)->is_down[Controller::left_shoulder],
		           right = (*c)->is_down[Controller::right] || (*c)->is_down[Controller::right_shoulder];
		
		x_direction += (left ? -1 : 0) + (right ? 1 : 0);
		
		// skip-level cheat
		if ((*c)->is_down[Controller::up] &&
		    (*c)->is_down[Controller::down] &&
		    (*c)->is_down[Controller::left] &&
		    (*c)->is_down[Controller::right])
			state = cheat_won;
	}
	
	// limit how hard player can push the ball
	x_direction = min(max(-1, x_direction), 1);
	
	highscore.push_back_tick(x_direction);
	
	for (vector<Ball>::iterator ball = balls.begin(); ball != balls.end(); ++ball)
	{
		ball->tick(x_direction);
		
		check_unboost(*ball);
		
		for (vector<Block>::iterator block = level.blocks.begin(); block != level.blocks.end(); ++block)
			if (!block->ignore)
				check_collide(*ball, *block);
		
		// check ball outside level
		if (state == none && is_outside(*ball, level))
		{
			state = lost;
			
			play_sample(samples["lost"], 1, sample_pan(ball->x));
		}
	}
}

void Game::check_unboost( Ball &ball )
{
	if (ball.can_unboost)
	{
		if (ball.user_can_unboost)
		{
			// if boost is not time-based, user can cancel the boost
			// by pushing ball in opposite direction
			if ((ball.was_pushed_left() && ball.is_moving_right()) ||
			    (ball.was_pushed_right() && ball.is_moving_left()))
			{
				ball.unboost();
				
				play_sample(samples["unboost"], 1, sample_pan(ball.x));
			}
		}
		else if (--ball.ticks_until_unboost == 0)
			ball.unboost();
	}
}

void Game::check_collide( Ball &ball, Block &block )
{
redo:
	bool x_in = (int)ball.x + ball.width > block.x &&
	            (int)ball.x < block.x + block.width;
	bool y_in = (int)ball.y + ball.height > block.y &&
	            (int)ball.y < block.y + block.height;
	
	if (x_in && y_in)
	{
		if (block.collideable)
		{
			// keep ball outside blocks
			if (fabsf(ball.y_vel) > 1)
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
		else if (block.type == Block::exit && state != quit && state != won)
		{
			state = won;
			
			play_sample(samples["won"], 1, sample_pan(ball.x));
		}
	}
	
	if (!block.collideable)
		return;
	
	Sample *sample = NULL;
	
	bool hit_top = false;
	
	if (x_in)
	{
		hit_top         = ball.is_moving_down() && (int)ball.y + ball.height == block.y;
		bool hit_bottom = ball.is_moving_up() && (int)ball.y == block.y + block.height;
		
		if (hit_top || hit_bottom)
		{
			if (ball.can_unboost)
				ball.unboost();
			
			ball.y_vel = hit_top ? -ball.y_term_vel : -ball.y_vel / 3;
			
			sample = &samples["bounce"];
		}
	}
	
	if (y_in)
	{
		bool hit_left  = ball.is_moving_right() && (int)ball.x + ball.width == block.x;
		bool hit_right = ball.is_moving_left() && (int)ball.x == block.x + block.width;
		
		if (hit_left || hit_right)
		{
			if (ball.can_unboost)
				ball.unboost();
			
			ball.x_vel = -ball.x_vel / 3;
			
			// wall jump
			if ((hit_left && ball.was_pushed_left()) ||
			    (hit_right && ball.was_pushed_right()))
			{
				ball.wall_jump();
				
				sample = &samples["wall_jump"];
			}
			else if (fabsf(ball.x_vel) > 3 * ball.push_x_accel)
			{
				sample = &samples["bounce"];
			}
		}
	}
	
	if (hit_top)
	{
		switch (block.type)
		{
		case Block::nuke:
			ball.no_vel = true;
			
			// TODO explode block and/or ball into particles
			
			state = lost;
			
			sample = &samples["nuke"];
			break;
			
		case Block::recycle:
			block.ignore = true;
			
			// TODO explode block into particles
			
			sample = &samples["recycle"];
			break;
			
		case Block::boost_up:
			ball.y_boost(-ball.y_boost_block);
			
			sample = &samples["boost"];
			break;
			
		case Block::boost_left:
			ball.x = block.x - Ball::width;
			ball.y += Ball::height;
			ball.x_boost(-ball.x_boost_block);
			
			sample = &samples["boost"];
			break;
			
		case Block::boost_right:
			ball.x = block.x + Block::width;
			ball.y += Ball::height;
			ball.x_boost(ball.x_boost_block);
			
			sample = &samples["boost"];
			break;
			
		default:
			break;
		}
	}
	
	if (sample != NULL)
		play_sample(*sample, 1, sample_pan(ball.x));
}

bool Game::is_outside( const Ball &ball, const Level &level ) const
{
	return (int)ball.x + ball.width <= 0 || (int)ball.x >= level.width ||
	       (int)ball.y + ball.height <= 0 || (int)ball.y >= level.height;
}

void Game::menu( void )
{
	SimpleMenu menu;
	const string entries[] =
	{
		"Continue",
		"Quit",
	};
	for (uint i = 0; i < COUNTOF(entries); ++i)
		menu.entries.push_back(entries[i]);
	
	menu.loop(SDL_GetVideoSurface());
	
	if (!menu.no_selection)
	{
		switch (menu.selection)
		{
		case 0:
			break;
		case 1:
			state = quit;
			break;
		}
	}
}
