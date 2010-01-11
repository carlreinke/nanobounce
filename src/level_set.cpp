#include "audio.hpp"
#include "file_system.hpp"
#include "font.hpp"
#include "game.hpp"
#include "level_set.hpp"
#include "main.hpp"
#include "volume.hpp"

using namespace std;

LevelSet::LevelSet( const std::string &directory )
: valid(false), directory(directory)
{
	const string meta_path = directory + "/" + "meta";
	
	if (path_exists(meta_path))
	{
		ifstream meta(meta_path.c_str());
		getline(meta, name);
		getline(meta, author);
		
		valid = meta.good();
	}
}

void LevelSet::load_levels( void )
{
	if (invalid())
		return;
	
	const string meta_path = directory + "/" + "meta";
	ifstream meta(meta_path.c_str());
	
	getline(meta, name);
	getline(meta, author);
	
	string level_path;
	while (getline(meta, level_path))
	{
		level_path = directory + "/" + level_path;
		
		Level level;
		level.load(level_path);
		
		if (!level.invalid())
			levels.push_back(level);
	}
}

void LevelSet::play( SDL_Surface *surface )
{
	if (invalid())
		return;
	
	if (levels.empty())
		load_levels();
	
	Game game;
	
	vector<Level>::iterator level = levels.begin();
	
	string highscore_path;
	Highscore highscore;
	
	while (game.state != Game::quit && level != levels.end() && !global_quit)
	{
		if (game.state == Game::lost)
		{
			// retry level
			game.reset();
		}
		else
		{
			game = Game(*level);
			
			highscore_path = level->path + ".score";
			highscore.load(highscore_path);
			
			LevelIntroLoop level_intro(*level, highscore);
			level_intro.loop(surface);
		}
		
		game.loop(surface);
		
		if (game.state == Game::won)
		{
			if (game.highscore.ms() < highscore.ms() || highscore.invalid())
			{
				// do highscore screen, ask for name?
				
				game.highscore.save(highscore_path);
			}
		}
		
		if (game.state == Game::won || game.state == Game::cheat_won)
			++level;
	}
	
	if (level == levels.end())
	{
		CongratsLoop congrats(*this);
		congrats.loop(surface);
	}
}

LevelSet::CongratsLoop::CongratsLoop( const LevelSet &level_set )
: set_name(level_set.name)
{
	play_music(music_directory + "special/Vulpine Skyflight.ogg");
}

void LevelSet::CongratsLoop::handle_event( SDL_Event &e )
{
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case Controller::back_key:
		case Controller::quit_key:
		case Controller::select_key:
		case Controller::start_key:
			loop_quit = true;
			break;
			
		default:
			break;
		}
		
	default:
		break;
	}
}

void LevelSet::CongratsLoop::update( void )
{
	for (vector< boost::shared_ptr<Controller> >::iterator c = controllers.begin(); c != controllers.end(); ++c)
		(*c)->update();
	
	// TODO: fix: fireworks require old tick value, so they only tick every other time
	static int i = 0;
	if (++i % 2 != 0)
		return;
	
	// firework colors
	static const SDL_Color red = SDL_Color_RGBA(224, 32, 32),
	                       yellow = SDL_Color_RGBA(224, 160, 32),
	                       green = SDL_Color_RGBA(32, 224, 64),
	                       blue = SDL_Color_RGBA(64, 32, 224),
	                       purple = SDL_Color_RGBA(224, 32, 192),
	                       white = SDL_Color_RGBA(255, 255, 255);
	static const SDL_Color *preset_colors[][3] =
	{
		// solids
		{ &red, &red, &red },
		{ &green, &green, &green },
		{ &blue, &blue, &blue },
		{ &white, &white, &white },
		
		// almost solids
		{ &red, &red, &yellow },
		{ &blue, &blue, &purple },
		
		// white highlights
		{ &red, &red, &white },
		{ &green, &green, &white },
		{ &blue, &blue, &white },
		{ &purple, &purple, &white },
		
		{ &red, &green, &blue },
		{ &red, &white, &blue },  // yay, US
	};
	
	// random fireworks
	if (rand() % 20 == 0)
	{
		const int x = rand() % (screen_width - screen_width / 4) + (screen_width / 8),
		          y = rand() % (screen_height - screen_height / 3) + (screen_height / 8);
		
		const SDL_Color **colors = preset_colors[(x + y) % COUNTOF(preset_colors)];
		
		for (uint i = 0; i < 40; ++i)
		{
			const Fixed radius = (Fixed(rand() % (1024 * 2) - 1024) / 1024),
			            angle = (Fixed(rand() % 31415) / 10000),
			            x_vel = cosf(angle) * radius,
			            y_vel = sinf(angle) * radius;
			
			particles.push_back(FireworkParticle(x, y, x_vel, y_vel, *colors[i % COUNTOF(*preset_colors)]));
		}
		
		play_sample(samples[NUKE], Fixed(1) / 5, Fixed(x) / screen_width);
	}
	
	Particle::tick_all(particles);
}

void LevelSet::CongratsLoop::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	for (list<Particle>::const_iterator i = particles.begin(); i != particles.end(); ++i)
		i->draw(surface, 0, 0, alpha);
	
	font.blit(surface, surface->w / 2, surface->h / 4, "Congratulations!", font_sprites[3], Font::majuscule, Font::center, alpha);
	font.blit(surface, surface->w / 2, surface->h / 2, set_name, font_sprites[4], Font::center, alpha);
	font.blit(surface, surface->w / 2, surface->h / 2 + font.height(font_sprites[4]), "completed!", font_sprites[3], Font::majuscule, Font::center, alpha);
}

LevelIntroLoop::LevelIntroLoop( const Level &level, const Highscore &score )
: level_name(level.name), score(score),
  ticks(0)
{
	// nothing to do
}

void LevelIntroLoop::handle_event( SDL_Event &e )
{
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case Controller::back_key:
		case Controller::quit_key:
		case Controller::select_key:
		case Controller::start_key:
			loop_quit = true;
			break;
			
		default:
			break;
		}
		
	default:
		break;
	}
}

void LevelIntroLoop::update( void )
{
	for (vector< boost::shared_ptr<Controller> >::iterator c = controllers.begin(); c != controllers.end(); ++c)
		(*c)->update();
	
	if (++ticks == 50)
		loop_quit = true;
}

void LevelIntroLoop::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	font.blit(surface, surface->w / 2, surface->h / 2 - font.height(font_sprites[3]), level_name, font_sprites[3], Font::center, alpha);
	
	if (!score.invalid())
	{
		font.blit(surface, surface->w / 2, surface->h * 3 / 4 - font.height(font_sprites[2]), "Best Time", font_sprites[2], Font::majuscule, Font::center, alpha);
		font.blit(surface, surface->w / 2, surface->h * 3 / 4, /*score.name + ": " +*/ score.time(), font_sprites[2], Font::center, alpha);
	}
}
