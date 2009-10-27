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
				case SDLK_BACKQUOTE:
				case SDLK_ESCAPE:
				case SDLK_SPACE:
				case SDLK_RETURN:
					fader.fade(Fader::out);
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
			
			fader.update();
			done = fader.is_done() && fader.was_fading(Fader::out);
			
			update_volume_notification();
		}
		
		if (frames--)
		{
			SDL_FillRect(surface, NULL, 0);
			
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

