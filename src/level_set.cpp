#include "audio.hpp"
#include "game.hpp"
#include "level_set.hpp"
#include "main.hpp"
#include "volume.hpp"

using namespace std;

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

void pack_done_screen( SDL_Surface *surface, const string &pack_name )
{
	Fader fader(20);
	fader.fade(Fader::in);
	
	std::list<Particle> particles;
	
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
				case Controller::back_key:
				case Controller::quit_key:
				case Controller::select_key:
				case Controller::start_key:
					fader.fade(Fader::out);
					break;
					
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
			
			// random fireworks
			if (rand() % 20 == 0)
			{
				const int x = rand() % (screen_width - screen_width / 4) + (screen_width / 8),
				          y = rand() % (screen_height - screen_height / 3) + (screen_height / 8);
				
				const SDL_Color colors[] =
				{
					SDL_Color_RGBA(224, 64, 64),   // red
					SDL_Color_RGBA(224, 224, 64),  // yellow
					SDL_Color_RGBA(64, 224, 64),   // green
					SDL_Color_RGBA(64, 64, 224),   // blue
					SDL_Color_RGBA(224, 64, 224),  // violet
					SDL_Color_RGBA(255, 255, 255), // white
				};
				const SDL_Color &color = colors[(x + y) % COUNTOF(colors)];
				
				for (uint i = 0; i < 40; ++i)
				{
					const Fixed radius = (Fixed(rand() % (1024 * 2) - 1024) / 1024),
					            angle = (Fixed(rand() % 31415) / 10000),
					            x_vel = cosf(angle) * radius,
					            y_vel = sinf(angle) * radius;
					
					particles.push_back(FireworkParticle(x, y, x_vel, y_vel, color));
				}
				
				play_sample(samples["nuke"], Fixed(1) / 5, Fixed(x) / screen_width);
			}
			
			Particle::tick_all(particles);
			
			fader.update();
			done = fader.is_done() && fader.was_fading(Fader::out);
			
			update_volume_notification();
		}
		
		if (frames--)
		{
			SDL_FillRect(surface, NULL, 0);
			
			for (list<Particle>::const_iterator i = particles.begin(); i != particles.end(); ++i)
				i->draw(surface, 0, 0, fader.value());
			
			font.blit(surface, surface->w / 2, surface->h / 4, "Congratulations!", font_sprites[3], Font::majuscule, Font::center, fader.value());
			font.blit(surface, surface->w / 2, surface->h / 2, pack_name, font_sprites[4], Font::center, fader.value());
			font.blit(surface, surface->w / 2, surface->h / 2 + font.height(font_sprites[4]), "completed!", font_sprites[3], Font::majuscule, Font::center, fader.value());
			
			draw_volume_notification(surface);
			
			SDL_Flip(surface);
			
			if (frames > 0)
				clog << "dropped " << frames << " frame(s)" << endl;
		}
	}
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

