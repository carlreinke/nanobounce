#include "audio.h"
#include "bouncing.h"
#include "controller.h"
#include "game.h"
#include "video.h"

using namespace std;

void play_pack( SDL_Surface *surface, const string &path )
{
	string pack_name, author;
	
	string filename;
	filename = path + "meta";
	
	ifstream pack_data(filename.c_str());
	getline(pack_data, pack_name);
	getline(pack_data, author);
	
	bool next_level = true;
	
	Level level;
	do
	{
		if (next_level)
		{
			string temp;
			getline(pack_data, temp);
			
			if (pack_data.fail())
			{
				pack_done_screen(surface, pack_name);
				return;
			}
			
			filename = path + temp;
		}
		
		cout << "loading '" << filename << "'" << endl;
		ifstream level_data(filename.c_str());
		level.load(level_data);
		
		if (level.state == Level::load_failed)
			continue;
		
		if (next_level)
			level_screen(surface, level);
		
		level_loop(surface, level);
		
		next_level = level.state == Level::won;
	}
	while (level.state != Level::quit && !global_quit);
}

void pack_done_screen( SDL_Surface *surface, const string &pack_name )
{
	int fade = SDL_ALPHA_TRANSPARENT;
	
	bool done = false, quit = false;
	while (!quit && !global_quit)
	{
		SDL_Event e;
		SDL_WaitEvent(&e);
		
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
			default:
				break;
			}
			break;
			
		case SDL_USEREVENT:
			switch (e.user.code)
			{
			case USER_FRAME:
				SDL_FillRect(surface, NULL, 0);
				
				font.blit(surface, screen_width / 2, screen_height / 4, "Congratulations!", font_sprites[3], Font::center, fade);
				font.blit(surface, screen_width / 2, screen_height / 2, pack_name, font_sprites[4], Font::center, fade);
				font.blit(surface, screen_width / 2, screen_height / 2 + font.height(font_sprites[4]), "Completed!", font_sprites[3], Font::center, fade);
				
				SDL_Flip(surface);
				break;
				
			case USER_UPDATE:
				for (vector<Controller *>::iterator c = controllers.begin(); c != controllers.end(); ++c)
					(*c)->update();
				
				if (done)
					fade = max(SDL_ALPHA_TRANSPARENT, fade - 20);
				else if (fade < SDL_ALPHA_OPAQUE)
					fade = min(fade + 20, SDL_ALPHA_OPAQUE);
				
				if (fade == SDL_ALPHA_TRANSPARENT)
					quit = true;
				
				break;
			}
		}
	}
}

void level_screen( SDL_Surface *surface, const Level &level )
{
	int ticks = 50, fade = SDL_ALPHA_TRANSPARENT;
	
	bool quit = false;
	while (!quit && !global_quit)
	{
		SDL_Event e;
		SDL_WaitEvent(&e);
		
		switch (e.type)
		{
		case SDL_QUIT:
			global_quit = true;
			break;
			
		case SDL_USEREVENT:
			switch (e.user.code)
			{
			case USER_FRAME:
				SDL_FillRect(surface, NULL, 0);
				
				font.blit(surface, screen_width / 2, screen_height / 2 - font.height(font_sprites[3]), level.name, font_sprites[3], Font::center, fade);
				
				SDL_Flip(surface);
				break;
				
			case USER_UPDATE:
				for (vector<Controller *>::iterator c = controllers.begin(); c != controllers.end(); ++c)
					(*c)->update();
				
				if (--ticks <= 0)
					fade = max(SDL_ALPHA_TRANSPARENT, fade - 20);
				else if (fade < SDL_ALPHA_OPAQUE)
					fade = min(fade + 20, SDL_ALPHA_OPAQUE);
				
				if (fade == SDL_ALPHA_TRANSPARENT)
					quit = true;
				
				break;
			}
		}
	}
}

void level_loop( SDL_Surface *surface, Level &level )
{
	int show_volume_ticks = 0;
	ostringstream volume_text;
	
	while (!level.done && !global_quit)
	{
		SDL_Event e;
		SDL_WaitEvent(&e);
		
		switch (e.type)
		{
		case SDL_QUIT:
			global_quit = true;
			break;
			
		case SDL_KEYDOWN:
			switch (e.key.keysym.sym)
			{
			case SDLK_PLUS:
			case SDLK_MINUS:
			{
				const Fixed delta = (e.key.keysym.sym == SDLK_MINUS) ? -0.1f : 0.1f;
				volume = min(max(0, volume + delta), 1);
				
				show_volume_ticks = 20;
				volume_text.str("");
				volume_text << (int)(volume * 100) << "%";
				cout << "volume: " << volume_text.str() << endl;
				break;
			}
			default:
				break;
			}
			break;
			
		case SDL_USEREVENT:
			switch (e.user.code)
			{
			case USER_FRAME:
				level.draw(surface);
				
				if (show_volume_ticks > 0)
					font.blit(surface, 0, screen_height - font.height(font_sprites[3]), volume_text.str(), font_sprites[3], Font::left, 128);
				
				SDL_Flip(surface);
				break;
				
			case USER_UPDATE:
				if (show_volume_ticks > 0)
					--show_volume_ticks;
				
				for (int i = 0; i < 4; ++i)
				{
					int x_direction = 0;
					
					for (vector<Controller *>::iterator c = controllers.begin(); c != controllers.end(); ++c)
					{
						(*c)->update();
						
						x_direction += (*c)->is_down[Controller::left] ? -1 : 0;
						x_direction += (*c)->is_down[Controller::right] ? 1 : 0;
						
						if ((*c)->is_down[Controller::quit])
							level.state = Level::quit;
					}
					
					level.update(x_direction);
				}
				break;
			}
			break;
			
		default:
			break;
		}
	}
}
