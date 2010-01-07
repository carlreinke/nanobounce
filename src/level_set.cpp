#include "audio.hpp"
#include "file_system.hpp"
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

void LevelSet::play( SDL_Surface *surface )
{
	assert(valid);
	
	const string meta_path = directory + "/" + "meta";
	ifstream meta(meta_path.c_str());
	
	getline(meta, name);
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
			getline(meta, level_path);
			level_path = directory + "/" + level_path;
			
			if (!meta.good())  // if end of level list
			{
				CongratsLoop congrats(name);
				congrats.loop(surface);
				return;
			}
			
			cout << "loading '" << level_path << "'" << endl;
			
			if (!game.load(level_path))
				continue;
			
			highscore_path = level_path + ".score";
			
			cout << "loading '" << highscore_path << "'" << endl;
			
			ifstream highscore_data(highscore_path.c_str());
			highscore.load(highscore_data);
			
			level_screen(surface, game.level, highscore);
		}
		
		game.loop(surface);
		
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

CongratsLoop::CongratsLoop( const std::string &pack_name )
: pack_name(pack_name)
{
	// nothing to do
}

void CongratsLoop::handle_event( SDL_Event &e )
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

void CongratsLoop::update( void )
{
	for (vector<Controller *>::iterator c = controllers.begin(); c != controllers.end(); ++c)
		(*c)->update();
	
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
		
		play_sample(samples["nuke"], Fixed(1) / 5, Fixed(x) / screen_width);
	}
	
	Particle::tick_all(particles);
}

void CongratsLoop::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	for (list<Particle>::const_iterator i = particles.begin(); i != particles.end(); ++i)
		i->draw(surface, 0, 0, alpha);
	
	font.blit(surface, surface->w / 2, surface->h / 4, "Congratulations!", font_sprites[3], Font::majuscule, Font::center, alpha);
	font.blit(surface, surface->w / 2, surface->h / 2, pack_name, font_sprites[4], Font::center, alpha);
	font.blit(surface, surface->w / 2, surface->h / 2 + font.height(font_sprites[4]), "completed!", font_sprites[3], Font::majuscule, Font::center, alpha);
}

void level_screen( SDL_Surface *surface, const Level &level, const Highscore &highscore )
{
	Fader fader(20);
	fader.fade(Fader::in);
	
	int ticks = 50;
	
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
				case Controller::vol_up_key:
					trigger_volume_change(0.1f);
					break;
				case Controller::vol_down_key:
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
			
			if (--ticks == 0)
				fader.fade(Fader::out);
			
			fader.update();
			done = fader.is_done() && fader.was_fading(Fader::out);
			
			update_volume_notification();
		}
		
		if (frames--)
		{
			SDL_FillRect(surface, NULL, 0);
			
			font.blit(surface, surface->w / 2, surface->h / 2 - font.height(font_sprites[3]), level.name, font_sprites[3], Font::center, fader.value());
			
			if (!highscore.invalid())
			{
				font.blit(surface, surface->w / 2, surface->h * 3 / 4 - font.height(font_sprites[2]), "Best Time", font_sprites[2], Font::majuscule, Font::center, fader.value());
				font.blit(surface, surface->w / 2, surface->h * 3 / 4, highscore.name + ": " + highscore.time(), font_sprites[2], Font::center, fader.value());
			}
			
			draw_volume_notification(surface);
			
			SDL_Flip(surface);
			
			if (frames > 0)
				clog << "dropped " << frames << " frame(s)" << endl;
		}
	}
}

