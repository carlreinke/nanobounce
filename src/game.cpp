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
			if (state != quit)
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
	
	for (list<Particle>::const_iterator i = particles.begin(); i != particles.end(); ++i)
		i->draw(surface, x_offset, y_offset, alpha);
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
			balls.push_back(Ball(b->x + (Block::width - Ball::width) / 2,
			                     b->y + (Block::height - Ball::height) / 2));
		}
	}
	
	state = none;
	
	x_offset = 0; //-(level.width - screen_width) / 2;
	y_offset = 0; //-(level.height - screen_height) / 2;
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
		
		// if ball outside level, it's dead
		if (state == none && is_outside(*ball, level))
		{
			state = lost;
			
			play_sample(samples["lost"], 1, sample_pan(ball->x));
		}
		else  // level view panning
		{
			if (level.width > screen_width)
			{
				if (ball->x + x_offset < screen_width / 4)  // ball in pan-left zone
					x_offset = min(static_cast<int>(x_offset + ceilf(ball->x_term_vel)), 0);
				else if (ball->x + x_offset > screen_width - screen_width / 4)  // ball in pan-right zone
					x_offset = max(screen_width - level.width, static_cast<int>(x_offset - ceilf(ball->x_term_vel)));
			}
			
			if (level.height > screen_height)
			{
				if (ball->y + y_offset < screen_height / 4)  // ball in pan-up zone
					y_offset = min(static_cast<int>(y_offset + ceilf(ball->y_term_vel)), 0);
				else if (ball->y + y_offset > screen_height - screen_height / 4)  // ball in pan-down zone
					y_offset = max(screen_height - level.height, static_cast<int>(y_offset - ceilf(ball->y_term_vel)));
			}
		}
	}
	
	Particle::tick_all(particles);
}

void Game::check_unboost( Ball &ball )
{
	if (ball.can_unboost)
	{
		if (ball.user_can_unboost)
		{
			// if boost is not time-bound, user can cancel the boost by pushing ball in opposite direction
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
	bool x_in = static_cast<int>(ball.x) + ball.width > block.x &&
	            static_cast<int>(ball.x) < block.x + block.width;
	bool y_in = static_cast<int>(ball.y) + ball.height > block.y &&
	            static_cast<int>(ball.y) < block.y + block.height;
	
	if (x_in && y_in)  // if ball inside block
	{
		if (block.collidable)
		{
			// force ball outside block
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
		else
		{
			// handle ball interaction with non-collidable blocks
			switch (block.type)
			{
			case Block::exit:
				if (state != quit && state != won)
				{
					state = won;
					ball.no_accel = true;
					// TODO: maybe trap ball inside block?
					
					play_sample(samples["won"], 1, sample_pan(ball.x));
				}
				break;
				
			case Block::push_up:
				ball.y_vel -= ball.y_accel * 3 / 2;
				
				if (SDL_GetTicks() % 2 == 0)
					particles.push_back(SparkParticle(ball.x, ball.y, -ball.x_vel, -ball.y_vel, (SDL_GetTicks() % 10 == 0) ?  SDL_Color_RGBA(128, 128, 255) : SDL_Color_RGBA(0, 0, 255)));
				
				break;
			case Block::push_left:
				ball.x_vel -= ball.y_accel / 2;
				
				if (SDL_GetTicks() % 2 == 0)
					particles.push_back(SparkParticle(ball.x, ball.y, -ball.x_vel, -ball.y_vel, (SDL_GetTicks() % 10 == 0) ?  SDL_Color_RGBA(128, 128, 255) : SDL_Color_RGBA(0, 0, 255)));
				
				break;
			case Block::push_right:
				ball.x_vel += ball.y_accel / 2;
				
				if (SDL_GetTicks() % 2 == 0)
					particles.push_back(SparkParticle(ball.x, ball.y, -ball.x_vel, -ball.y_vel, (SDL_GetTicks() % 10 == 0) ?  SDL_Color_RGBA(128, 128, 255) : SDL_Color_RGBA(0, 0, 255)));
				
				break;
				
			default:
				break;
			}
		}
	}
	
	if (!block.collidable)  // rest of function deals with collisions
		return;
	
	// play no more than one sample per tick, otherwise both "bounce" and "nuke" might happen in the same tick, for example
	Sample *sample = NULL;
	
	bool hit_top = false;
	
	if (x_in)
	{
		hit_top         = ball.is_moving_down() && static_cast<int>(ball.y) + ball.height == block.y;
		bool hit_bottom = ball.is_moving_up() && static_cast<int>(ball.y) == block.y + block.height;
		
		if (hit_top || hit_bottom)
		{
			if (ball.can_unboost)
				ball.unboost();
			
			ball.y_vel = hit_top ? -ball.y_term_vel : max(-ball.y_vel, ball.y_term_vel / 2) / 3;
			
			sample = &samples["bounce"];
		}
	}
	
	if (y_in)
	{
		bool hit_left  = ball.is_moving_right() && static_cast<int>(ball.x) + ball.width == block.x;
		bool hit_right = ball.is_moving_left() && static_cast<int>(ball.x) == block.x + block.width;
		
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
				
				for (int i = 0; i < 10; ++i)
					particles.push_back(SparkParticle(ball.x, ball.y, ball.x_vel, ball.y_vel, SDL_Color_RGBA(255, 255, 255)));
				
				sample = &samples["wall_jump"];
			}
			else if (fabsf(ball.x_vel) > 3 * ball.push_x_accel)  // if ball has enough force
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
			state = lost;
			ball.no_vel = true;
			block.ignore = true;
			
			for (int y = 0; y < Block::height; y += 3)
				for (int x = 0; x < Block::width; x += 3)
					particles.push_back(ExplosionParticle(block.x + x, block.y + y));
			
			for (int i = 0; i < 18; ++i)
				particles.push_back(SparkParticle(ball.x, ball.y, i / 6 - 1, ball.y_vel + i % 2, SDL_Color_RGBA(255, 255, 255)));
			for (int i = 0; i < 9; ++i)
				particles.push_back(SparkParticle(ball.x, ball.y, i / 3 - 1, ball.y_vel + i % 2, SDL_Color_RGBA(255, 128, 0)));
			
			sample = &samples["nuke"];
			break;
			
		case Block::recycle:
			block.ignore = true;
			
			for (int y = 0; y < Block::height; y += 3)
				for (int x = 0; x < Block::width; x += 3)
					particles.push_back(DustParticle(block.x + x, block.y + y));
			
			sample = &samples["recycle"];
			break;
			
		case Block::boost_up:
			ball.y_boost(-ball.y_boost_block);
			
			for (int i = 0; i < 10; ++i)
				particles.push_back(SparkParticle(ball.x, ball.y, ball.x_vel, ball.y_vel, SDL_Color_RGBA(0, 255, 0)));
			
			sample = &samples["boost"];
			break;
			
		case Block::boost_left:
			ball.x = block.x - Ball::width;
			ball.y += Ball::height;
			ball.x_boost(-ball.x_boost_block);
			
			for (int i = 0; i < 10; ++i)
				particles.push_back(SparkParticle(ball.x, ball.y, ball.x_vel, ball.y_vel, SDL_Color_RGBA(0, 255, 0)));
			
			sample = &samples["boost"];
			break;
			
		case Block::boost_right:
			ball.x = block.x + Block::width;
			ball.y += Ball::height;
			ball.x_boost(ball.x_boost_block);
			
			for (int i = 0; i < 10; ++i)
				particles.push_back(SparkParticle(ball.x, ball.y, ball.x_vel, ball.y_vel, SDL_Color_RGBA(0, 255, 0)));
			
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
	return static_cast<int>(ball.x) + ball.width <= 0 || static_cast<int>(ball.x) >= level.width ||
	       static_cast<int>(ball.y) + ball.height <= 0 || static_cast<int>(ball.y) >= level.height;
}

void Game::menu( void )
{
	SimpleMenu menu;
	const string entries[] =
	{
		"Continue",
		"Restart",
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
			reset();
			break;
		case 2:
			state = quit;
			break;
		}
	}
}
