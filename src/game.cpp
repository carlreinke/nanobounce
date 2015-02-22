#include "audio/audio.hpp"
#include "file_system.hpp"
#include "game.hpp"
#include "game_loops.hpp"
#include "main.hpp"
#include "menu.hpp"
#include "misc.hpp"

using std::max;
using std::min;

Game::Samples Game::samples;

Game::Game( const Level &level, Controllers controllers )
: state(NONE),
  level_hash(level.calculate_hash()),
  width(level.get_width()), height(level.get_height()),
  play_controllers(controllers)
{
	for (const LevelBlock &block : level.get_blocks())
		blocks.push_back(GameBlock(block));
	
	reset();
}

void Game::loop( SDL_Surface *surface )
{
	reset();

	Loop::loop(surface);
}

void Game::handle_event( SDL_Event &e )
{
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case Controller::START_KEY:
			if (state != QUIT)
				menu();
			break;
		case Controller::QUIT_KEY:
			state = QUIT;
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
	for (std::shared_ptr<Controller> &controller : controllers)
		controller->update();
	
	if (!fader.is_fading(Fader::IN))
	{
		// update replay controllers
		for (std::shared_ptr<Controller> &controller : play_controllers)
			controller->tick_update();
		
		tick();
	}
	
	if (state != NONE)
		loop_quit = true;
}

void Game::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	for (const GameBlock &block : blocks)
		block.draw(surface, x_offset, y_offset, alpha);
	
	for (const Ball &ball : balls)
		ball.draw(surface, x_offset, y_offset, alpha);
	
	for (const Particle &particle : particles)
		particle.draw(surface, x_offset, y_offset, alpha);
}

void Game::reset( void )
{
	highscore = Highscore(level_hash);
	
	for (GameBlock &block : blocks)
		block.ingame_reset();
	
	int ball_x = 0, ball_y = 0;
	
	balls.clear();
	for (GameBlock &block : blocks)
	{
		if (block.ingame_type == GameBlock::BALL)
		{
			balls.push_back(Ball(block.x + (GameBlock::width - Ball::width) / 2,
			                     block.y + (GameBlock::height - Ball::height) / 2));
			
			ball_x += balls.back().x;
			ball_y += balls.back().y;
		}
	}
	
	state = NONE;
	
	// initial pan
	x_offset = -(width - screen_width) / 2;
	y_offset = -(height - screen_height) / 2;
	
	// adjust pan so that ball is on screen
	if (balls.size() > 0)
	{
		ball_x /= balls.size();
		x_offset = -min(max(-x_offset, ball_x - screen_width), ball_x);
		
		ball_y /= balls.size();
		y_offset = -min(max(-y_offset, ball_y - screen_height), ball_y);
	}
	
	particles.clear();
}

void Game::tick( void )
{
	int x_direction = 0;
	
	for (std::shared_ptr<Controller> &c : play_controllers)
	{
		const bool left = c->is_down[Controller::LEFT] || c->is_down[Controller::LEFT_SHOULDER];
		const bool right = c->is_down[Controller::RIGHT] || c->is_down[Controller::RIGHT_SHOULDER];
		
		x_direction += (left ? -1 : 0) + (right ? 1 : 0);
		
		// skip-level cheat
		if (c->is_down[Controller::UP] &&
		    c->is_down[Controller::DOWN] &&
		    c->is_down[Controller::LEFT_SHOULDER] &&
		    c->is_down[Controller::RIGHT_SHOULDER])
		{
			if (state == NONE)
				state = CHEAT_WON;
		}
	}
	
	// limit how hard player can push the ball
	x_direction = min(max(-1, x_direction), 1);
	
	highscore.push_back_tick(x_direction);
	
	for (Ball &ball : balls)
	{
		ball.tick(x_direction);
		check_collide(ball);
		
		check_unboost(ball);
		
		// if ball outside level, it's dead
		if (!ball_overlaps_level(ball) && state == NONE)
		{
			state = LOST;
			
			highscore.save(user_data_directory / "last_lost.score");
			
			play_sample(samples.lost, 1, sample_pan(ball.x));
		}
		else  // level view panning
		{
			if (width > screen_width)
			{
				if (ball.x + x_offset < screen_width / 4)  // ball in pan-left zone
					x_offset = min(static_cast<int>(x_offset + ceilf(ball.x_term_vel)), 0);
				else if (ball.x + x_offset > screen_width - screen_width / 4)  // ball in pan-right zone
					x_offset = max(screen_width - width, static_cast<int>(x_offset - ceilf(ball.x_term_vel)));
			}
			
			if (height > screen_height)
			{
				if (ball.y + y_offset < screen_height / 4)  // ball in pan-up zone
					y_offset = min(static_cast<int>(y_offset + ceilf(ball.y_term_vel)), 0);
				else if (ball.y + y_offset > screen_height - screen_height / 4)  // ball in pan-down zone
					y_offset = max(screen_height - height, static_cast<int>(y_offset - ceilf(ball.y_term_vel)));
			}
		}
	}
	
	Particle::tick_all(particles);
}

void Game::load_resources( void )
{
	boost::array<std::pair<Sample *, std::string>, 9> sample_names =
	{{
		std::make_pair(&samples.bounce,    "bounce"),
		std::make_pair(&samples.unboost,   "unboost"),
		std::make_pair(&samples.wall_jump, "wall_jump"),
		
		std::make_pair(&samples.boost,     "boost"),
		std::make_pair(&samples.nuke,      "nuke"),
		std::make_pair(&samples.recycle,   "recycle"),
		std::make_pair(&samples.toggle,    "toggle"),
		
		std::make_pair(&samples.lost,      "lost"),
		std::make_pair(&samples.won,       "won"),
	}};
	
	typedef std::pair<Sample *, std::string> SamplePair;
	for (const SamplePair &i : sample_names)
	{
		if (i.first->empty())
			*i.first = Sample((sample_directory / (i.second + ".ogg")).string());
	}
}

bool Game::check_collide( Ball &ball, int recursion_depth )
{
	GameBlock *block_hit = NULL;
	Fixed order = 0;
	Fixed revert_x, revert_y;
	bool hit_x, hit_y;
	Fixed edge_dist_x, edge_dist_y;
	
	if (recursion_depth >= 100)
	{
		highscore.save(user_data_directory / "bug_report.score");
		assert(false);
		return false;
	}
	
#if defined(DEBUG_COLLIDE)
	std::cout << std::fixed;
	std::cout << "pre"
	          << "\tx " << std::setprecision(8) << ball.x
	          << "\ty " << std::setprecision(8) << ball.y
	          << "\tvx " << std::setprecision(8) << ball.x_vel
	          << "\tvy " << std::setprecision(8) << ball.y_vel
	          << std::endl;
#endif
	
	// check each block for collision
	for (GameBlock &block : blocks)
	{
		if (block.ingame_property == GameBlock::COLLIDABLE)
		{
			Fixed revert_x_temp, revert_y_temp;
			bool hit_x_temp, hit_y_temp;
			Fixed edge_dist_x_temp, edge_dist_y_temp;
			Fixed order_temp = collision_depth_fraction(ball, block, revert_x_temp, revert_y_temp, hit_x_temp, hit_y_temp, edge_dist_x_temp, edge_dist_y_temp);
			
			if (order_temp > order)  // ball and block collide, and collision is deepest seen yet
			{
				// store collision for processing
				block_hit = &block;
				order = order_temp;
				revert_x = revert_x_temp;
				revert_y = revert_y_temp;
				hit_x = hit_x_temp;
				hit_y = hit_y_temp;
				edge_dist_x = edge_dist_x_temp;
				edge_dist_y = edge_dist_y_temp;
			}
			
#if defined(DEBUG_COLLIDE)
			if (order_temp > 0)
				std::cout << "? " << &block << " " << (hit_x_temp ? "x" : " ") << (hit_y_temp ? "y" : " ")
				          << "\to " << std::setprecision(8) << order_temp
				          << "\trx " << std::setprecision(8) << revert_x
				          << "\try " << std::setprecision(8) << revert_y
				          << std::endl;
#endif
		}
	}
	
	// found collision, revert ball to edge
	if (order > 0)
	{
#if defined(DEBUG_COLLIDE)
		std::cout << block_hit << " " << (hit_x ? "x" : " ") << (hit_y ? "y" : " ")
		          << "\to " << std::setprecision(8) <<  order
		          << "\trx " << std::setprecision(8) << revert_x
		          << "\try " << std::setprecision(8) << revert_y
		          << std::endl;
#endif
		
		if (hit_x)
		{
			if (!hit_y && fabsf(edge_dist_y) < make_frac<Fixed>(1, 2))
			{
				// prevent wall climbing via corner collisions
				// if ball is in one-half-unit-deep corner-collision with block, try to reposition it to reduce false collisions
				ball.y -= edge_dist_y;
				ball.trail.back().y = ball.y + make_frac<Fixed>(1, 2);
				
				return check_collide(ball, recursion_depth + 1);
			}
			else
			{
				ball.trail.back().x = ball.x - revert_x + make_frac<Fixed>(1, 2);
				ball.x -= 2 * revert_x;
				
				// ball was repositioned, check again
				if (!check_collide(ball, recursion_depth + 1))
				{
					// no other collisions found, so handle this one
					handle_block_x_collision(ball);
					return false;
				}
			}
		}
		if (hit_y)
		{
			if (!hit_x && fabsf(edge_dist_x) < make_frac<Fixed>(1, 2))
			{
				ball.x -= edge_dist_x;
				ball.trail.back().x = ball.x + make_frac<Fixed>(1, 2);
				
				return check_collide(ball, recursion_depth + 1);
			}
			else
			{
				ball.trail.back().y = ball.y - revert_y + make_frac<Fixed>(1, 2);
				ball.y -= 2 * revert_y;
				
				// ball was repositioned, check again
				if (!check_collide(ball, recursion_depth + 1))
				{
					// no other collisions found, so handle this one
					handle_block_y_collision(ball, *block_hit);
					return false;
				}
			}
		}
	}
	else  // no collisions left
	{
		// check triggerable blocks
		for (const GameBlock &block : blocks)
		{
			if (block.ingame_property == GameBlock::TRIGGERABLE && ball_half_inside_block(ball, block))
			{
				handle_noncollidable_block(ball, block);
			}
		}
	}
	
#if defined(DEBUG_COLLIDE)
	std::cout << "post"
	          << "\tx " << std::setprecision(8) << ball.x
	          << "\ty " << std::setprecision(8) << ball.y
	          << "\tvx " << std::setprecision(8) << ball.x_vel
	          << "\tvy " << std::setprecision(8) << ball.y_vel
	          << std::endl
	          << std::endl;
#endif
	
	return (order > 0);
}

inline Fixed Game::collision_depth_fraction( const Ball &ball, const GameBlock &block, Fixed &revert_x, Fixed &revert_y, bool &hit_x, bool &hit_y, Fixed &edge_dist_x, Fixed &edge_dist_y ) const
{
	const Fixed past_left   = ball.x + Fixed(ball.width)   - block.x;
	const Fixed past_right  = ball.x - Fixed(block.width)  - block.x;
	const Fixed past_top    = ball.y + Fixed(ball.height)  - block.y;
	const Fixed past_bottom = ball.y - Fixed(block.height) - block.y;
	
	const bool in_x = past_left > 0 && past_right < 0;
	const bool in_y = past_top > 0 && past_bottom < 0;
	
	if (in_x && in_y)
	{
		const Fixed &past_x = ball.is_moving_right() ? past_left : past_right;
		const Fixed &past_y = ball.is_moving_down() ? past_top : past_bottom;
		
		const Fixed frac_past_x = ball.x_vel == 0 ? Fixed(0) : past_x / ball.x_vel;
		const Fixed frac_past_y = ball.y_vel == 0 ? Fixed(0) : past_y / ball.y_vel;
		
		if (frac_past_x > 1 && frac_past_y > 1)  // both invalid
		{
			highscore.save(user_data_directory / "bug_report.score");
			assert(false);
			return 0;
		}
		else
		{
			hit_x = (frac_past_x >= frac_past_y && frac_past_x <= 1) || frac_past_y > 1,
			hit_y = (frac_past_y >= frac_past_x && frac_past_y <= 1) || frac_past_x > 1;
			
			assert(hit_x || hit_y);
			
			edge_dist_x = fabsf(past_left) < fabsf(past_right) ? past_left : past_right;
			edge_dist_y = fabsf(past_top) < fabsf(past_bottom) ? past_top : past_bottom;
			
			if (hit_x)
			{
				revert_x = past_x;
				revert_y = ball.y_vel * frac_past_x;
			}
			else
			{
				revert_x = ball.x_vel * frac_past_y;
				revert_y = past_y;
			}
			
			return sqr(past_x) + sqr(past_y);
		}
	}
	
	return 0;
}

inline void Game::handle_block_x_collision( Ball &ball )
{
	Sample *sample = NULL;
	
	const bool hit_left  = ball.is_moving_right();
	const bool hit_right = ball.is_moving_left();
	
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

inline void Game::handle_block_y_collision( Ball &ball, GameBlock &block )
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
		switch (block.ingame_type)
		{
			int temp;
			
		case GameBlock::NUKE:
			if (state == NONE)
			{
				state = LOST;
				
				highscore.save(user_data_directory / "last_lost.score");
			}
			
			ball.no_vel = true;
			
			for (int y = 0; y < block.height; y += 3)
				for (int x = 0; x < block.width; x += 3)
					particles.push_back(ExplosionParticle(block.x + x, block.y + y));
			
			for (int i = 0; i < 18; ++i)
				particles.push_back(SparkParticle(ball.x, ball.y, i / 6 - 1, ball.y_vel + i % 2, SDL_Color_RGBA(255, 255, 255)));
			for (int i = 0; i < 9; ++i)
				particles.push_back(SparkParticle(ball.x, ball.y, i / 3 - 1, ball.y_vel + i % 2, SDL_Color_RGBA(255, 128, 0)));
			
			sample = &samples.nuke;
			break;
			
		case GameBlock::RECYCLE:
			block.ingame_property = GameBlock::HIDDEN;
			
			for (int y = 0; y < block.height; y += 3)
				for (int x = 0; x < block.width; x += 3)
					particles.push_back(DustParticle(block.x + x, block.y + y));
			
			sample = &samples.recycle;
			break;
			
		case GameBlock::TOGGLE_0_STAR:
			temp = 0;
			
			for (GameBlock &block : blocks)
			{
				const bool is_toggle_0 = block.ingame_type == GameBlock::TOGGLE_0_0;
				
				if (is_toggle_0 && !ball_overlaps_block(ball, block))
				{
					block.ingame_type = GameBlock::TOGGLE_0_1;
					block.ingame_property = GameBlock::COLLIDABLE;
					
					for (int y = 0; y < block.height; y += 5)
						for (int x = 0; x < block.width; x += 5)
							particles.push_back(StarDustParticle(block.x + x, block.y + y, SDL_Color_RGBA(0, 255, 255)));
					
					++temp;
				}
			}
			
			if (temp > 0)
				sample = &samples.toggle;
			break;
			
		case GameBlock::TOGGLE_1_STAR:
			for (GameBlock &block : blocks)
			{
				const bool is_toggle_1 = block.ingame_type == GameBlock::TOGGLE_1_1 || block.ingame_type == GameBlock::TOGGLE_1_STAR;
				
				if (is_toggle_1)
				{
					block.ingame_type = GameBlock::TOGGLE_1_0;
					block.ingame_property = GameBlock::IGNORED;
					
					for (int y = 0; y < block.height; y += 5)
						for (int x = 0; x < block.width; x += 5)
							particles.push_back(StarDustParticle(block.x + x, block.y + y, SDL_Color_RGBA(255, 255, 0)));
				}
			}
			
			sample = &samples.toggle;
			break;
			
		case GameBlock::BOOST_UP:
			ball.y_boost(-ball.y_boost_block);
			
			for (int i = 0; i < 10; ++i)
				particles.push_back(SparkParticle(ball.x, ball.y, ball.x_vel, ball.y_vel, SDL_Color_RGBA(0, 255, 0)));
			
			sample = &samples.boost;
			break;
			
		case GameBlock::BOOST_LEFT:
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
			
		case GameBlock::BOOST_RIGHT:
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
inline void Game::handle_noncollidable_block( Ball &ball, const GameBlock &block )
{
	switch (block.ingame_type)
	{
	case GameBlock::EXIT:
		if (state == NONE)
		{
			state = WON;
			ball.no_accel = true;
			// TODO: maybe trap ball inside block?
			
			highscore.save(user_data_directory / "last_won.score");
			
			play_sample(samples.won, 1, sample_pan(ball.x));
		}
		break;
		
	case GameBlock::PUSH_UP:
		ball.y_vel -= ball.y_accel * 3 / 2;
		
		if (SDL_GetTicks() % 2 == 0)
			particles.push_back(SparkParticle(ball.x, ball.y, -ball.x_vel, -ball.y_vel, (SDL_GetTicks() % 10 == 0) ?  SDL_Color_RGBA(192, 192, 255) : SDL_Color_RGBA(0, 0, 255)));
		
		break;
	case GameBlock::PUSH_LEFT:
		ball.x_vel -= ball.y_accel / 2;
		
		if (SDL_GetTicks() % 2 == 0)
			particles.push_back(SparkParticle(ball.x, ball.y, -ball.x_vel, -ball.y_vel, (SDL_GetTicks() % 10 == 0) ?  SDL_Color_RGBA(192, 192, 255) : SDL_Color_RGBA(0, 0, 255)));
		
		break;
	case GameBlock::PUSH_RIGHT:
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
		bool tick_based = (ball.ticks_until_unboost > 0);
		
		// user can cancel the boost by pushing ball in opposite direction
		if ((tick_based && --ball.ticks_until_unboost == 0) ||
		    (ball.was_pushed_left() && ball.is_moving_right()) ||
		    (ball.was_pushed_right() && ball.is_moving_left()))
		{
			ball.unboost();
			
			if (!tick_based)
				play_sample(samples.unboost, 1, sample_pan(ball.x));
		}
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
	return ball_overlaps_rect(ball, 0, 0, width, height);
}

// check if ball is in collision with a block
inline bool Game::ball_overlaps_block( const Ball &ball, const GameBlock &block )
{
	return ball_overlaps_rect(ball, block.x, block.y, block.width, block.height);
}

// check if ball is at least half inside a block
inline bool Game::ball_half_inside_block( const Ball &ball, const GameBlock &block )
{
	const bool x_half_in = ball.x + Fixed(ball.width) / 2 >= block.x &&
	                       ball.x + Fixed(ball.width) / 2 < block.x + block.width;
	const bool y_half_in = ball.y + Fixed(ball.height) / 2 >= block.y &&
	                       ball.y + Fixed(ball.height) / 2 < block.y + block.height;
	
	return (x_half_in && y_half_in);
}

// check if ball is in collision with any collidable block
bool Game::ball_overlaps_any_block( const Ball &ball ) const
{
	for (const GameBlock &block : blocks)
	{
		if (block.ingame_property == GameBlock::COLLIDABLE)
			if (ball_overlaps_block(ball, block))
				return true;
	}
	
	return false;
}

void Game::menu( void )
{
	SimpleMenu menu(surface);
	const std::string entries[] =
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
		case 0:  // Continue
			break;
		case 1:  // Restart
			state = RESTART;
			break;
		case 2:  // Quit
			state = QUIT;
			break;
		}
	}
}
