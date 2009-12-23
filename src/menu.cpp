#include "ball.hpp"
#include "controller.hpp"
#include "file_system.hpp"
#include "font.hpp"
#include "highscore.hpp"
#include "level_set.hpp"
#include "main.hpp"
#include "menu.hpp"
#include "misc.hpp"
#include "sdl_ext.hpp"
#include "volume.hpp"

using namespace std;

SimpleMenu::SimpleMenu( bool no_fade )
: Loop(no_fade),
  selection(0), no_selection(false)
{
	background = SDL_DuplicateRGBSurface(SDL_GetVideoSurface());
}

SimpleMenu::~SimpleMenu( void )
{
	SDL_FreeSurface(background);
}

void SimpleMenu::handle_event( SDL_Event &e )
{
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case Controller::back_key:
		case Controller::quit_key:
			no_selection = true;
		case Controller::select_key:
		case Controller::start_key:
			loop_quit = true;
			break;
			
		case Controller::left_shoulder_key:
		case Controller::left_key:
		case Controller::up_key:
			if (selection == 0)
				selection = entries.size();
			--selection;
			break;
			
		case Controller::right_shoulder_key:
		case Controller::right_key:
		case Controller::down_key:
			++selection;
			if (selection == entries.size())
				selection = 0;
			break;
			
		default:
			break;
		}
		break;
		
	default:
		break;
	}
}

void SimpleMenu::update( void )
{
	// update controller
	for (vector<Controller *>::iterator c = controllers.begin(); c != controllers.end(); ++c)
		(*c)->update();
}

void SimpleMenu::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	(void)alpha;
	
	SDL_FillRect(surface, NULL, 0);
	
	SDL_SetAlpha(background, SDL_SRCALPHA, SDL_ALPHA_OPAQUE - alpha / 2);
	SDL_BlitSurface(background, NULL, surface, NULL);
	
	const uint x = surface->w / 2;
	const uint delta_y = font.height(font_sprites[3]) + font.height(font_sprites[3]) / 4;
	uint y = (surface->h - delta_y * entries.size()) / 2;
	
	for (uint i = 0; i < entries.size(); ++i)
	{
		font.blit(surface, x, y, entries[i], font_sprites[3], Font::majuscule, Font::center, (i == selection) ? alpha : alpha / 2);
		y += delta_y;
	}
}

void game_menu( SDL_Surface *surface )
{
	unsigned int selection = 0;
	string menu_items[] =
	{
		"Play",
//		"Scores",
		"Quit"
	};
	
	Ball ball(surface->w / 2, surface->h);
	
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
				case Controller::back_key:
				case Controller::quit_key:
					quit = true;
					break;
					
				case Controller::left_key:
				case Controller::left_shoulder_key:
					if (selection > 0)
						--selection;
					break;
					
				case Controller::right_key:
				case Controller::right_shoulder_key:
					if (selection < (signed)COUNTOF(menu_items) - 1)
						++selection;
					break;
					
				case Controller::select_key:
				case Controller::start_key:
					switch (selection)
					{
					case 0:
						pack_menu(surface);
						break;
						
					case 1:
						quit = true;
						break;
					}
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
			
			for (int i = 0; i < 4; ++i)
			{
				int x = surface->w * ((Fixed)((signed)selection + 1) / ((signed)COUNTOF(menu_items) + 1)),
					y = surface->h - font.height(font_sprites[3]) * 2;
				
				ball.tick(ball.x > x ? -1 : 1);
				if (ball.y > y)
					ball.y_vel = -ball.y_term_vel;
			}
			
			update_volume_notification();
		}
		
		if (frames--)
		{
			SDL_FillRect(surface, NULL, 0);
			
			ball.draw(surface);
			
			font.blit(surface, surface->w / 2, surface->h / 2 - font.height(font_sprites[4]), "Nanobounce", font_sprites[4], Font::majuscule, Font::center);
			
			for (unsigned int i = 0; i < COUNTOF(menu_items); ++i)
			{
				int x = surface->w * ((Fixed)((signed)i + 1) / ((signed)COUNTOF(menu_items) + 1)),
					y = surface->h - font.height(font_sprites[3]) * 3;
				
				font.blit(surface, x, y, menu_items[i], font_sprites[3], Font::majuscule, Font::center, (i == selection) ? SDL_ALPHA_OPAQUE : 128);
			}
			
			font.blit(surface, 0, surface->h - font.height(font_sprites[1]), "v0.1 BETA", font_sprites[1], Font::majuscule, Font::left);
			
			font.blit(surface, surface->w - 1, surface->h - font.height(font_sprites[1]) * 2, "programming, graphics, and sound:", font_sprites[1], Font::majuscule, Font::right);
			font.blit(surface, surface->w - 1, surface->h - font.height(font_sprites[1]), "Carl \"Mindless\" Reinke", font_sprites[1], Font::majuscule, Font::right);
			
			draw_volume_notification(surface);
			
			SDL_Flip(surface);
			
			if (frames > 0)
				clog << "dropped " << frames << " frame(s)" << endl;
		}
	}
}

class Pack_Entry
{
public:
	Pack_Entry( string name, string author, string directory ) : name(name), author(author), directory(directory) {  }
	string name, author, directory;
	bool operator<( const Pack_Entry &that ) const { return this->name < that.name; }
};

void pack_menu( SDL_Surface *surface )
{
	string directory = "levels/";
	
	vector<Pack_Entry> packs;
	
	// populate the pack list
	{
		vector<string> entries = directory_listing(directory);
		
		for (vector<string>::const_iterator entry = entries.begin(); entry != entries.end(); ++entry)
		{
			string filename = directory + *entry + "/" + "meta";
			
			if (path_exists(filename))
			{
				string pack_name, author;
				
				ifstream pack_data(filename.c_str());
				getline(pack_data, pack_name);
				getline(pack_data, author);
				
				if (pack_data.good())
					packs.push_back(Pack_Entry(pack_name, author, *entry + "/"));
			}
		}
	}
	
	sort(packs.begin(), packs.end());
	
	unsigned int selection = 0;
	
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
				case Controller::back_key:
				case Controller::quit_key:
					quit = true;
					break;
					
				case Controller::up_key:
				case Controller::left_key:
				case Controller::left_shoulder_key:
					if (selection > 0)
						--selection;
					else
						selection = packs.size() - 1;
					break;
					
				case Controller::down_key:
				case Controller::right_key:
				case Controller::right_shoulder_key:
					if (selection < packs.size() - 1)
						++selection;
					else
						selection = 0;
					break;
					
				case Controller::select_key:
				case Controller::start_key:
					directory += packs[selection].directory;
					play_pack(surface, directory);
					
					quit = true;
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
			
			update_volume_notification();
		}
		
		if (frames--)
		{
			SDL_FillRect(surface, NULL, 0);
			
			// draw list
			{
				int y = font.height(font_sprites[3]) / 2;
				int p = (signed)selection - 4;
				
				for (unsigned int i = 0; i < 4; ++i)
				{
					if (p >= 0)
						font.blit(surface, surface->w / 2, y, packs[p].name, font_sprites[3], Font::center, 128);
					y += font.height(font_sprites[3]);
					++p;
				}
				
				y += font.height(font_sprites[3]) / 2;
				font.blit(surface, surface->w / 2, y, packs[p].name, font_sprites[3], Font::center);
				y += font.height(font_sprites[3]);
				font.blit(surface, surface->w / 2, y, packs[p].author, font_sprites[1], Font::center);
				y += font.height(font_sprites[1]);
				y += font.height(font_sprites[3]) / 2;
				++p;
				
				for (unsigned int i = 0; i < 4; ++i)
				{
					if (p < (signed)packs.size())
						font.blit(surface, surface->w / 2, y, packs[p].name, font_sprites[3], Font::center, 128);
					y += font.height(font_sprites[3]);
					++p;
				}
			}
			
			draw_volume_notification(surface);
			
			SDL_Flip(surface);
			
			if (frames > 0)
				clog << "dropped " << frames << " frame(s)" << endl;
		}
	}
}
