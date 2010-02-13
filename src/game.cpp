#include "audio/audio.hpp"
#include "game.hpp"
#include "main.hpp"
#include "menu.hpp"
#include "misc.hpp"

using namespace std;

Game::Samples Game::samples;

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
	BOOST_FOREACH (boost::shared_ptr<Controller> &controller, controllers)
		controller->update();
	
	if (!fader.is_fading(Fader::in))
	{
		for (uint i = 0; i < ups_multiplier; ++i)
		{
			// update replay controllers
			BOOST_FOREACH (boost::shared_ptr<Controller> &controller, controllers)
				controller->tick_update();
			
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
	
	BOOST_FOREACH (const Ball &ball, balls)
		ball.draw(surface, x_offset, y_offset, alpha);
	
	BOOST_FOREACH (const Particle &particle, particles)
		particle.draw(surface, x_offset, y_offset, alpha);
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
	highscore.reset();
	
	int ball_x = 0, ball_y = 0;
	
	balls.clear();
	BOOST_FOREACH (const Block &block, level.blocks)
	{
		if (block.type == Block::ball)
		{
			balls.push_back(Ball(block.x + (Block::width - Ball::width) / 2,
			                     block.y + (Block::height - Ball::height) / 2));
			
			ball_x += balls.back().x;
			ball_y += balls.back().y;
		}
	}
	
	state = none;
	
	// initial pan
	x_offset = -(level.width - screen_width) / 2;
	y_offset = -(level.height - screen_height) / 2;
	
	// adjust pan so that ball is on screen
	if (balls.size() > 0)
	{
		ball_x /= balls.size();
		x_offset = -min(max(-x_offset, ball_x - screen_width), ball_x);
		
		ball_y /= balls.size();
		y_offset = -min(max(-y_offset, ball_y - screen_height), ball_y);
	}
}

void Game::tick( void )
{
	int x_direction = 0;
	
	BOOST_FOREACH (boost::shared_ptr<Controller> &c, controllers)
	{
		const bool left = c->is_down[Controller::left] || c->is_down[Controller::left_shoulder],
		           right = c->is_down[Controller::right] || c->is_down[Controller::right_shoulder];
		
		x_direction += (left ? -1 : 0) + (right ? 1 : 0);
		
		// skip-level cheat
		if (c->is_down[Controller::up] &&
		    c->is_down[Controller::down] &&
		    c->is_down[Controller::left_shoulder] &&
		    c->is_down[Controller::right_shoulder])
		{
			if (state == none)
				state = cheat_won;
		}
	}
	
	// limit how hard player can push the ball
	x_direction = min(max(-1, x_direction), 1);
	
	highscore.push_back_tick(x_direction);
	
	BOOST_FOREACH (Ball &ball, balls)
	{
		ball.tick(x_direction);
		check_collide(ball);
		
		check_unboost(ball);
		
		// if ball outside level, it's dead
		if (!ball_overlaps_level(ball) && state == none)
		{
			state = lost;
			
			highscore.save("last_lost.score");
			
			play_sample(samples.lost, 1, sample_pan(ball.x));
		}
		else  // level view panning
		{
			if (level.width > screen_width)
			{
				if (ball.x + x_offset < screen_width / 4)  // ball in pan-left zone
					x_offset = min(static_cast<int>(x_offset + ceilf(ball.x_term_vel)), 0);
				else if (ball.x + x_offset > screen_width - screen_width / 4)  // ball in pan-right zone
					x_offset = max(screen_width - level.width, static_cast<int>(x_offset - ceilf(ball.x_term_vel)));
			}
			
			if (level.height > screen_height)
			{
				if (ball.y + y_offset < screen_height / 4)  // ball in pan-up zone
					y_offset = min(static_cast<int>(y_offset + ceilf(ball.y_term_vel)), 0);
				else if (ball.y + y_offset > screen_height - screen_height / 4)  // ball in pan-down zone
					y_offset = max(screen_height - level.height, static_cast<int>(y_offset - ceilf(ball.y_term_vel)));
			}
		}
	}
	
	Particle::tick_all(particles);
}

void Game::load_resources( void )
{
	boost::array<pair<Sample *, string>, 9> sample_names =
	{{
		make_pair(&samples.bounce,    "bounce"),
		make_pair(&samples.unboost,   "unboost"),
		make_pair(&samples.wall_jump, "wall_jump"),
		
		make_pair(&samples.boost,     "boost"),
		make_pair(&samples.nuke,      "nuke"),
		make_pair(&samples.recycle,   "recycle"),
		make_pair(&samples.toggle,    "toggle"),
		
		make_pair(&samples.lost,      "lost"),
		make_pair(&samples.won,       "won"),
	}};
	
	typedef pair<Sample *, string> SamplePair;
	BOOST_FOREACH (const SamplePair &i, sample_names)
	{
		if (i.first->empty())
			*i.first = Sample(sample_directory + i.second + ".ogg");
	}
}

void Game::check_collide( Ball &ball )
{
	Block *block_hit = NULL;
	Fixed vel_revert_frac = 0;
	Fixed revert_x, revert_y;
	bool hit_x, hit_y;
	
/*	cout << fixed;
	cout << "pre"
	     << "\tx " << setprecision(8) << ball.x
	     << "\ty " << setprecision(8) << ball.y
	     << endl;*/
	
	// check each block for collision
	BOOST_FOREACH (Block &block, level.blocks)
	{
		if (block.property == Block::collidable)
		{
			Fixed revert_x_temp, revert_y_temp;
			bool hit_x_temp, hit_y_temp;
			Fixed temp = collision_depth_fraction(ball, block, revert_x_temp, revert_y_temp, hit_x_temp, hit_y_temp);
			
			if (temp > vel_revert_frac)  // ball and block collide, and collision is deepest seen yet
			{
				// store collision for processing
				block_hit = &block;
				vel_revert_frac = temp;
				revert_x = revert_x_temp;
				revert_y = revert_y_temp;
				hit_x = hit_x_temp;
				hit_y = hit_y_temp;
			}
			
/*			if (temp > 0)
				cout << "? " << &block << " " << (hit_x_temp ? "x" : " ") << (hit_y_temp ? "y" : " ")
				     << "\t% " << setprecision(8) << temp
				     << "\trx " << setprecision(8) << revert_x
				     << "\try " << setprecision(8) << revert_y
				     << endl;*/
		}
		else if (block.property == Block::triggerable && ball_half_inside_block(ball, block))
		{
			handle_noncollidable_block(ball, block);
		}
	}
	
	// found collision, revert ball to edge
	if (vel_revert_frac > 0)
	{
/*		cout << block_hit << " " << (hit_x ? "x" : " ") << (hit_y ? "y" : " ")
		     << "\t% " << setprecision(8) << vel_revert_frac
		     << "\trx " << setprecision(8) << revert_x
		     << "\try " << setprecision(8) << revert_y
		     << endl;*/
		
		// prevent wall climbing via corner collisions
		if (hit_y && (fabsf(ball.x + ball.width - block_hit->x) < make_frac<Fixed>(1, 2) ||
		              fabsf(ball.x - block_hit->x - block_hit->width) < make_frac<Fixed>(1, 2)))
		{
			ball.x -= revert_x;
		}
		else
		if (hit_x && (fabsf(ball.y + ball.height - block_hit->y) < make_frac<Fixed>(1, 2) ||
		              fabsf(ball.y - block_hit->y - block_hit->height) < make_frac<Fixed>(1, 2)))
		{
			ball.y -= revert_y;
		}
		else
		{
			if (hit_x)
			{
				ball.trail.back().first = ball.x - revert_x + make_frac<Fixed>(1, 2);
				ball.x -= 2 * revert_x;
				
				handle_block_x_collision(ball);
			}
			if (hit_y)
			{
				ball.trail.back().second = ball.y - revert_y + make_frac<Fixed>(1, 2);
				ball.y -= 2 * revert_y;
				
				handle_block_y_collision(ball, *block_hit);
			}
		}
		
		// ball was repositioned, check again
		check_collide(ball);
	}
	
/*	cout << "post"
		 << "\tx " << setprecision(8) << ball.x
		 << "\ty " << setprecision(8) << ball.y
		 << endl
		 << endl;*/
}

inline Fixed Game::collision_depth_fraction( const Ball &ball, const Block &block, Fixed &revert_x, Fixed &revert_y, bool &hit_x, bool &hit_y ) const
{
	const Fixed past_left   = ball.x + Fixed(ball.width)   - block.x,
	            past_right  = ball.x - Fixed(block.width)  - block.x,
	            past_top    = ball.y + Fixed(ball.height)  - block.y,
	            past_bottom = ball.y - Fixed(block.height) - block.y;
	
	const bool in_x = past_left > 0 && past_right < 0,
	           in_y = past_top > 0 && past_bottom < 0;
	
	if (in_x && in_y)
	{
		const Fixed &past_x = ball.is_moving_right() ? past_left : past_right,
		            &past_y = ball.is_moving_down() ? past_top : past_bottom;
		
		const Fixed frac_past_x = ball.x_vel == 0 ? Fixed(0) : past_x / ball.x_vel,
		            frac_past_y = ball.y_vel == 0 ? Fixed(0) : past_y / ball.y_vel;
		
		if (frac_past_x > 1 && frac_past_y > 1)  // both invalid
		{
			return 0;
		}
		else
		{
			hit_x = (frac_past_x >= frac_past_y && frac_past_x <= 1) || frac_past_y > 1,
			hit_y = (frac_past_y >= frac_past_x && frac_past_y <= 1) || frac_past_x > 1;
			
			assert(hit_x || hit_y);
			
			if (hit_x)
			{
				revert_x = past_x;
				revert_y = ball.x_vel == 0 ? Fixed(0) : ball.y_vel * past_x / ball.x_vel;
				return frac_past_x;
			}
			else
			{
				revert_x = ball.y_vel == 0 ? Fixed(0) : ball.x_vel * past_y / ball.y_vel;
				revert_y = past_y;
				return frac_past_y;
			}
		}
	}
	
	return 0;
}

inline void Game::handle_block_x_collision( Ball &ball )
{
	Sample *sample = NULL;
	
	const bool hit_left  = ball.is_moving_right(),
	           hit_right = ball.is_moving_left();
	
	if (ball.can_unboost)
	{
		ball.unboost();
		ball.x_vel = -ball.x_vel;
	}
	else
	{
		ball.x_vel = -ball.x_vel * make_frac<Fixed>(2, 3);
	}
	
	if (fabsf(ball.x_vel) > 3 * ball.push_x_accel)  // if ball has enough force
	{
		// wall jump
		if ((hit_left && ball.was_pushed_left()) ||
		    (hit_right && ball.was_pushed_right()))
		{
			ball.wall_jump();
			
			for (int i = 0; i < 10; ++i)
				particles.push_back(SparkParticle(ball.x, ball.y, ball.x_vel, ball.y_vel, SDL_Color_RGBA(255, 255, 255)));
			
			sample = &samples.wall_jump;
		}
		else
		{
			// prevent wall-jumping within one-block area
			if (ball.was_pushed_left() || ball.was_pushed_right())
				ball.x_vel = min(max(-ball.x_term_vel, ball.x_vel), ball.x_term_vel) / 2;
			
			sample = &samples.bounce;
		}
	}
	
	if (sample != NULL)
		play_sample(*sample, 1, sample_pan(ball.x));
}

inline void Game::handle_block_y_collision( Ball &ball, Block &block )
{
	// play no more than one sample per tick, otherwise both "bounce" and "nuke" might happen in the same tick, for example
	Sample *sample = NULL;
	
	const bool hit_top = ball.is_moving_down();
	
	if (ball.can_unboost)
		ball.unboost();
	
	ball.y_vel = hit_top ? -ball.y_term_vel : max(-ball.y_vel, ball.y_term_vel / 2) / 3;
	
	sample = &samples.bounce;
	
	if (hit_top)
	{
		switch (block.type)
		{
			int temp;
			
		case Block::nuke:
			if (state == none)
				state = lost;
			
			ball.no_vel = true;
			block.property = Block::hidden;
			
			for (int y = 0; y < block.height; y += 3)
				for (int x = 0; x < block.width; x += 3)
					particles.push_back(ExplosionParticle(block.x + x, block.y + y));
			
			for (int i = 0; i < 18; ++i)
				particles.push_back(SparkParticle(ball.x, ball.y, i / 6 - 1, ball.y_vel + i % 2, SDL_Color_RGBA(255, 255, 255)));
			for (int i = 0; i < 9; ++i)
				particles.push_back(SparkParticle(ball.x, ball.y, i / 3 - 1, ball.y_vel + i % 2, SDL_Color_RGBA(255, 128, 0)));
			
			sample = &samples.nuke;
			break;
			
		case Block::recycle:
			block.property = Block::hidden;
			
			for (int y = 0; y < block.height; y += 3)
				for (int x = 0; x < block.width; x += 3)
					particles.push_back(DustParticle(block.x + x, block.y + y));
			
			sample = &samples.recycle;
			break;
			
		case Block::toggle_0_star:
			temp = 0;
			
			BOOST_FOREACH (Block &block, level.blocks)
			{
				const bool is_toggle_0 = block.type == Block::toggle_0_0;
				
				if (is_toggle_0 && !ball_overlaps_block(ball, block))
				{
					block.type = Block::toggle_0_1;
					block.property = Block::collidable;
					
					for (int y = 0; y < block.height; y += 5)
						for (int x = 0; x < block.width; x += 5)
							particles.push_back(StarDustParticle(block.x + x, block.y + y, SDL_Color_RGBA(0, 255, 255)));
					
					++temp;
				}
			}
			
			if (temp > 0)
				sample = &samples.toggle;
			break;
			
		case Block::toggle_1_star:
			BOOST_FOREACH (Block &block, level.blocks)
			{
				const bool is_toggle_1 = block.type == Block::toggle_1_1 || block.type == Block::toggle_1_star;
				
				if (is_toggle_1)
				{
					block.type = Block::toggle_1_0;
					block.property = Block::ignored;
					
					for (int y = 0; y < block.height; y += 5)
						for (int x = 0; x < block.width; x += 5)
							particles.push_back(StarDustParticle(block.x + x, block.y + y, SDL_Color_RGBA(255, 255, 0)));
				}
			}
			
			sample = &samples.toggle;
			break;
			
		case Block::boost_up:
			ball.y_boost(-ball.y_boost_block);
			
			for (int i = 0; i < 10; ++i)
				particles.push_back(SparkParticle(ball.x, ball.y, ball.x_vel, ball.y_vel, SDL_Color_RGBA(0, 255, 0)));
			
			sample = &samples.boost;
			break;
			
		case Block::boost_left:
			{
				Ball temp(block.x - ball.width, block.y);
				
				if (!ball_overlaps_any_block(temp))
				{
					ball.x = temp.x;
					ball.y = temp.y;
					ball.x_boost(-ball.x_boost_block);
					
					for (int i = 0; i < 10; ++i)
						particles.push_back(SparkParticle(ball.x, ball.y, ball.x_vel, ball.y_vel, SDL_Color_RGBA(0, 255, 0)));
					
					sample = &samples.boost;
				}
			}
			break;
			
		case Block::boost_right:
			{
				Ball temp(block.x + block.width, block.y);
				
				if (!ball_overlaps_any_block(temp))
				{
					ball.x = temp.x;
					ball.y = temp.y;
					
					ball.x_boost(ball.x_boost_block);
					
					for (int i = 0; i < 10; ++i)
						particles.push_back(SparkParticle(ball.x, ball.y, ball.x_vel, ball.y_vel, SDL_Color_RGBA(0, 255, 0)));
					
					sample = &samples.boost;
				}
			}
			break;
			
		default:
			break;
		}
	}
	
	if (sample != NULL)
		play_sample(*sample, 1, sample_pan(ball.x));
}

// handle ball interaction with non-collidable blocks
inline void Game::handle_noncollidable_block( Ball &ball, Block &block )
{
	switch (block.type)
	{
	case Block::exit:
		if (state == none)
		{
			state = won;
			ball.no_accel = true;
			// TODO: maybe trap ball inside block?
			
			highscore.save("last_won.score");
			
			play_sample(samples.won, 1, sample_pan(ball.x));
		}
		break;
		
	case Block::push_up:
		ball.y_vel -= ball.y_accel * 3 / 2;
		
		if (SDL_GetTicks() % 2 == 0)
			particles.push_back(SparkParticle(ball.x, ball.y, -ball.x_vel, -ball.y_vel, (SDL_GetTicks() % 10 == 0) ?  SDL_Color_RGBA(192, 192, 255) : SDL_Color_RGBA(0, 0, 255)));
		
		break;
	case Block::push_left:
		ball.x_vel -= ball.y_accel / 2;
		
		if (SDL_GetTicks() % 2 == 0)
			particles.push_back(SparkParticle(ball.x, ball.y, -ball.x_vel, -ball.y_vel, (SDL_GetTicks() % 10 == 0) ?  SDL_Color_RGBA(192, 192, 255) : SDL_Color_RGBA(0, 0, 255)));
		
		break;
	case Block::push_right:
		ball.x_vel += ball.y_accel / 2;
		
		if (SDL_GetTicks() % 2 == 0)
			particles.push_back(SparkParticle(ball.x, ball.y, -ball.x_vel, -ball.y_vel, (SDL_GetTicks() % 10 == 0) ?  SDL_Color_RGBA(192, 192, 255) : SDL_Color_RGBA(0, 0, 255)));
		
		break;
		
	default:
		break;
	}
}

inline void Game::check_unboost( Ball &ball )
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
				
				play_sample(samples.unboost, 1, sample_pan(ball.x));
			}
		}
		else if (--ball.ticks_until_unboost == 0)
			ball.unboost();
	}
}

// check if ball is at least partially within a rectangle
inline bool Game::ball_overlaps_rect( const Ball &ball, int x, int y, int w, int h )
{
	const bool x_in = ball.x + ball.width > x &&
	                  ball.x < x + w;
	const bool y_in = ball.y + ball.height > y &&
	                  ball.y < y + h;
	
	return (x_in && y_in);
}

// check if ball is at least partially within level boundaries
inline bool Game::ball_overlaps_level( const Ball &ball ) const
{
	return ball_overlaps_rect(ball, 0, 0, level.width, level.height);
}

// check if ball is in collision with a block
inline bool Game::ball_overlaps_block( const Ball &ball, const Block &block )
{
	return ball_overlaps_rect(ball, block.x, block.y, block.width, block.height);
}

// check if ball is at least half inside a block
inline bool Game::ball_half_inside_block( const Ball &ball, const Block &block )
{
	const bool x_half_in = ball.x + Fixed(ball.width) / 2 >= block.x &&
	                       ball.x + Fixed(ball.width) / 2 < block.x + block.width;
	const bool y_half_in = ball.y + Fixed(ball.height) / 2 >= block.y &&
	                       ball.y + Fixed(ball.height) / 2 < block.y + block.height;
	
	return (x_half_in && y_half_in);
}

// check if ball is in collision with any collideable block
bool Game::ball_overlaps_any_block( const Ball &ball ) const
{
	BOOST_FOREACH (const Block &block, level.blocks)
	{
		if (block.property == Block::collidable)
			if (ball_overlaps_block(ball, block))
				return true;
	}
	
	return false;
}

void Game::menu( void )
{
	SimpleMenu menu(surface);
	const string entries[] =
	{
		"Continue",
		"Restart",
		"Quit",
	};
	for (uint i = 0; i < COUNTOF(entries); ++i)
		menu.entries.push_back(entries[i]);
	
	menu.loop(surface);
	
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
