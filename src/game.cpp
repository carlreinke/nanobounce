#include "audio.hpp"
#include "controller.hpp"
#include "game.hpp"
#include "main.hpp"
#include "video.hpp"
#include "volume.hpp"

using namespace std;

Game::Game( void )
: state(none)
{
	// good to go
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
		x_direction += (*c)->is_down[Controller::left] ? -1 : 0;
		x_direction += (*c)->is_down[Controller::right] ? 1 : 0;
		
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

void Game::draw( SDL_Surface *surface, Uint8 alpha )
{
	SDL_FillRect(surface, NULL, 0);
	
	level.draw(surface, x_offset, y_offset, alpha);
	
	for (vector<Ball>::const_iterator ball = balls.begin(); ball != balls.end(); ++ball)
		ball->draw(surface, x_offset, y_offset, alpha);
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
		else if (block.type == Block::exit && state != won)
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
		case Block::bomb:
			ball.no_vel = true;
			
			// TODO explode block and/or ball into particles
			
			state = lost;
			
			sample = &samples["explode"];
			break;
			
		case Block::cracked:
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

void play_pack( SDL_Surface *surface, const string &directory )
{
	string meta_path;
	meta_path = directory + "meta";
	ifstream meta(meta_path.c_str());
	
	string pack_name, author;
	getline(meta, pack_name);
	getline(meta, author);
	
	string level_path;
	Game game;
	
	string highscore_path;
	Highscore highscore;
	
	while (game.state != Game::quit && !global_quit)
	{
		if (game.state == Game::lost)
		{
			// retry level
			game.reset();
		}
		else
		{
			string level_file;
			getline(meta, level_file);
			
			if (!meta.good())
			{
				pack_done_screen(surface, pack_name);
				return;
			}
			
			level_path = directory + level_file;
			
			cout << "loading '" << level_path << "'" << endl;
			
			if (!game.load(level_path))
				continue;
			
			highscore_path = level_path + ".score";
			
			cout << "loading '" << highscore_path << "'" << endl;
			
			ifstream highscore_data(highscore_path.c_str());
			highscore.load(highscore_data);
			
			level_screen(surface, game.level, highscore);
		}
		
		level_loop(surface, game);
		
		if (game.state == Game::won)
		{
			if (game.highscore.ms() < highscore.ms() || highscore.invalid())
			{
				// do highscore screen, ask for name?
				
				cout << "saving new highscore '" << highscore_path << "'" << endl;
				
				ofstream highscore_data(highscore_path.c_str());
				game.highscore.save(highscore_data);
			}
		}
	}
}

void pack_done_screen( SDL_Surface *surface, const string &pack_name )
{
	int fade = SDL_ALPHA_TRANSPARENT;
	
	bool done = false, quit = false;
	while (!quit && !global_quit)
	{
		SDL_WaitEvent(NULL);
		
		int updates = 0, frames = 0;
		
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				global_quit = true;
				break;
				
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym)
				{
				case SDLK_BACKQUOTE:
				case SDLK_ESCAPE:
				case SDLK_SPACE:
				case SDLK_RETURN:
					done = true;
					break;
					
				case SDLK_PLUS:
					trigger_volume_change(0.1f);
					break;
				case SDLK_MINUS:
					trigger_volume_change(-0.1f);
					break;
					
				default:
					break;
				}
				break;
				
			case SDL_USEREVENT:
				switch (e.user.code)
				{
				case USER_UPDATE:
					++updates;
					break;
				case USER_FRAME:
					++frames;
					break;
				}
			}
		}
		
		while (updates--)
		{
			for (vector<Controller *>::iterator c = controllers.begin(); c != controllers.end(); ++c)
				(*c)->update();
			
			if (done)
				fade = max(SDL_ALPHA_TRANSPARENT, fade - 20);
			else if (fade < SDL_ALPHA_OPAQUE)
				fade = min(fade + 20, SDL_ALPHA_OPAQUE);
			
			if (fade == SDL_ALPHA_TRANSPARENT)
				quit = true;
			
			update_volume_notification();
		}
		
		if (frames--)
		{
			SDL_FillRect(surface, NULL, 0);
			
			font.blit(surface, surface->w / 2, surface->h / 4, "Congratulations!", font_sprites[3], Font::majuscule, Font::center, fade);
			font.blit(surface, surface->w / 2, surface->h / 2, pack_name, font_sprites[4], Font::center, fade);
			font.blit(surface, surface->w / 2, surface->h / 2 + font.height(font_sprites[4]), "completed!", font_sprites[3], Font::majuscule, Font::center, fade);
			
			draw_volume_notification(surface);
			
			SDL_Flip(surface);
			
			if (frames > 0)
				clog << "dropped " << frames << " frame(s)" << endl;
		}
	}
}

void level_screen( SDL_Surface *surface, const Level &level, const Highscore &highscore )
{
	int ticks = 50, fade = SDL_ALPHA_TRANSPARENT;
	
	bool quit = false;
	while (!quit && !global_quit)
	{
		SDL_WaitEvent(NULL);
		
		int updates = 0, frames = 0;
		
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				global_quit = true;
				break;
				
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym)
				{
				case SDLK_PLUS:
					trigger_volume_change(0.1f);
					break;
				case SDLK_MINUS:
					trigger_volume_change(-0.1f);
					break;
					
				default:
					break;
				}
				break;
				
			case SDL_USEREVENT:
				switch (e.user.code)
				{
				case USER_UPDATE:
					++updates;
					break;
				case USER_FRAME:
					++frames;
					break;
				}
			}
		}
		
		while (updates--)
		{
			for (vector<Controller *>::iterator c = controllers.begin(); c != controllers.end(); ++c)
				(*c)->update();
			
			if (--ticks <= 0)
				fade = max(SDL_ALPHA_TRANSPARENT, fade - 20);
			else if (fade < SDL_ALPHA_OPAQUE)
				fade = min(fade + 20, SDL_ALPHA_OPAQUE);
			
			if (fade == SDL_ALPHA_TRANSPARENT)
				quit = true;
			
			update_volume_notification();
		}
		
		if (frames--)
		{
			SDL_FillRect(surface, NULL, 0);
			
			font.blit(surface, surface->w / 2, surface->h / 2 - font.height(font_sprites[3]), level.name, font_sprites[3], Font::center, fade);
			
			if (!highscore.invalid())
			{
				font.blit(surface, surface->w / 2, surface->h * 3 / 4 - font.height(font_sprites[2]), "Best Time", font_sprites[2], Font::majuscule, Font::center, fade);
				font.blit(surface, surface->w / 2, surface->h * 3 / 4, highscore.name + ": " + highscore.time(), font_sprites[2], Font::center, fade);
			}
			
			draw_volume_notification(surface);
			
			SDL_Flip(surface);
			
			if (frames > 0)
				clog << "dropped " << frames << " frame(s)" << endl;
		}
	}
}

void level_loop( SDL_Surface *surface, Game &game )
{
	Fader fader;
	fader.fade(Fader::in);
	
	bool done = false;
	
	while (!done && !global_quit)
	{
		SDL_WaitEvent(NULL);
		
		int updates = 0, frames = 0;
		
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				global_quit = true;
				break;
				
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym)
				{
				case SDLK_RETURN:
					if (game.state == Game::none)
						game.state = Game::paused;
					else if (game.state == Game::paused)
						game.state = Game::none;
					break;
				case SDLK_ESCAPE:
					game.state = Game::quit;
					break;
					
				case SDLK_PLUS:
					trigger_volume_change(0.1f);
					break;
				case SDLK_MINUS:
					trigger_volume_change(-0.1f);
					break;
					
				default:
					break;
				}
				break;
				
			case SDL_USEREVENT:
				switch (e.user.code)
				{
				case USER_UPDATE:
					++updates;
					break;
				case USER_FRAME:
					++frames;
					break;
				}
				break;
				
			default:
				break;
			}
		}
		
		while (updates--)
		{
			fader.update();
			done = fader.is_done() && fader.was_fading(Fader::out);
			
			// update controller
			for (vector<Controller *>::iterator c = controllers.begin(); c != controllers.end(); ++c)
				(*c)->update();
			
			if (game.state != Game::paused && !fader.is_fading(Fader::in))
			{
				for (int i = 0; i < 4; ++i)
				{
					// update replay controllers
					for (vector<Controller *>::iterator c = controllers.begin(); c != controllers.end(); ++c)
						(*c)->tick_update();
					
					game.tick();
				}
			}
			
			if (game.state != Game::none && game.state != Game::paused)
				fader.fade(Fader::out);
				
			update_volume_notification();
		}
		
		if (frames--)
		{
			game.draw(surface, fader.value());
			
			// paused message
			if (game.state == Game::paused)
				font.blit(surface, surface->w / 2, surface->h / 2, "Paused", font_sprites[3], Font::majuscule, Font::center, 128);
			
			draw_volume_notification(surface);
			
			SDL_Flip(surface);
			
			if (frames > 0)
				clog << "dropped " << frames << " frame(s)" << endl;
		}
	}
}
