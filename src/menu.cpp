#include "ball.hpp"
#include "controller.hpp"
#include "font.hpp"
#include "game.hpp"
#include "main.hpp"
#include "menu.hpp"
#include "misc.hpp"
#include "video.hpp"

using namespace std;

void game_menu( SDL_Surface *surface )
{
	unsigned int selection = 0;
	string menu_items[] =
	{
		"Play",
//		"Scores",
		"Quit"
	};
	
	Ball ball(screen_width / 2, screen_height);
	
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
			
		case SDL_KEYDOWN:
			switch (e.key.keysym.sym)
			{
			case SDLK_BACKQUOTE:
			case SDLK_ESCAPE:
				quit = true;
				break;
				
			case SDLK_LEFT:
				if (selection > 0)
					--selection;
				break;
				
			case SDLK_RIGHT:
				if (selection < (signed)COUNTOF(menu_items) - 1)
					++selection;
				break;
				
			case SDLK_SPACE:
			case SDLK_RETURN:
				switch (selection)
				{
				case 0:
					pack_menu(surface);
					break;
					
				case 1:
					quit = true;
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
				SDL_FillRect(surface, NULL, 0);
				
				ball.draw(surface);
				
				font.blit(surface, screen_width / 2, screen_height / 2 - font.height(font_sprites[4]), "Nanobounce", font_sprites[4], Font::majuscule, Font::center);
				
				for (unsigned int i = 0; i < COUNTOF(menu_items); ++i)
				{
					int x = screen_width * ((Fixed)((signed)i + 1) / ((signed)COUNTOF(menu_items) + 1)),
						y = screen_height - font.height(font_sprites[3]) * 3;
					
					font.blit(surface, x, y, menu_items[i], font_sprites[3], Font::majuscule, Font::center, (i == selection) ? SDL_ALPHA_OPAQUE : 128);
				}
				
				font.blit(surface, 0, screen_height - font.height(font_sprites[1]), "v0.1 BETA", font_sprites[1], Font::majuscule, Font::left);
				
				font.blit(surface, screen_width - 1, screen_height - font.height(font_sprites[1]) * 2, "programming, graphics, and sound:", font_sprites[1], Font::majuscule, Font::right);
				font.blit(surface, screen_width - 1, screen_height - font.height(font_sprites[1]), "Carl \"Mindless\" Reinke", font_sprites[1], Font::majuscule, Font::right);
				
				SDL_Flip(surface);
				break;
				
			case USER_UPDATE:
				for (vector<Controller *>::iterator c = controllers.begin(); c != controllers.end(); ++c)
					(*c)->update();
				
				for (int i = 0; i < 4; ++i)
				{
					int x = screen_width * ((Fixed)((signed)selection + 1) / ((signed)COUNTOF(menu_items) + 1)),
						y = screen_height - font.height(font_sprites[3]) * 2;
					
					ball.tick(ball.x > x ? -1 : 1);
					if (ball.y > y)
						ball.y_vel = -ball.y_term_vel;
				}
				break;
			}
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
	string directory = "lvl/";
	
	vector<Pack_Entry> packs;
	
	// populate the pack list
	{
		DIR *dir = opendir(directory.c_str());
		if (dir == NULL)
			return;
		
		struct dirent *dir_ent;
		while ((dir_ent = readdir(dir)) != NULL)
		{
			string filename = directory + dir_ent->d_name + "/" + "meta";
			
			struct stat buffer;
			if (stat(filename.c_str(), &buffer) == 0)
			{
				string pack_name, author;
				
				ifstream pack_data(filename.c_str());
				getline(pack_data, pack_name);
				getline(pack_data, author);
				
				if (pack_data.good())
					packs.push_back(Pack_Entry(pack_name, author, string(dir_ent->d_name) + "/"));
			}
		}
		
		closedir(dir);
	}
	
	sort(packs.begin(), packs.end());
	
	unsigned int selection = 0;
	
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
			
		case SDL_KEYDOWN:
			switch (e.key.keysym.sym)
			{
			case SDLK_BACKQUOTE:
			case SDLK_ESCAPE:
				quit = true;
				break;
				
			case SDLK_UP:
			case SDLK_LEFT:
				if (selection > 0)
					--selection;
				else
					selection = packs.size() - 1;
				break;
				
			case SDLK_DOWN:
			case SDLK_RIGHT:
				if (selection < packs.size() - 1)
					++selection;
				else
					selection = 0;
				break;
				
			case SDLK_SPACE:
			case SDLK_RETURN:
				directory += packs[selection].directory;
				play_pack(surface, directory);
				
				quit = true;
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
				
				// draw list
				{
					int y = font.height(font_sprites[3]) / 2;
					int p = (signed)selection - 4;
					
					for (unsigned int i = 0; i < 4; ++i)
					{
						if (p >= 0)
							font.blit(surface, screen_width / 2, y, packs[p].name, font_sprites[3], Font::center, 128);
						y += font.height(font_sprites[3]);
						++p;
					}
					
					y += font.height(font_sprites[3]) / 2;
					font.blit(surface, screen_width / 2, y, packs[p].name, font_sprites[3], Font::center);
					y += font.height(font_sprites[3]);
					font.blit(surface, screen_width / 2, y, packs[p].author, font_sprites[1], Font::center);
					y += font.height(font_sprites[1]);
					y += font.height(font_sprites[3]) / 2;
					++p;
					
					for (unsigned int i = 0; i < 4; ++i)
					{
						if (p < (signed)packs.size())
							font.blit(surface, screen_width / 2, y, packs[p].name, font_sprites[3], Font::center, 128);
						y += font.height(font_sprites[3]);
						++p;
					}
				}
				
				SDL_Flip(surface);
				break;
				
			case USER_UPDATE:
				for (vector<Controller *>::iterator c = controllers.begin(); c != controllers.end(); ++c)
					(*c)->update();
				break;
			}
		}
	}
}
