#include "audio.h"
#include "controller.h"
#include "font.h"
#include "level.h"
#include "misc.h"
#include "video.h"
#include "SDL.h"

using namespace std;

void game_menu( SDL_Surface *surface );
void pack_menu( SDL_Surface *surface );

void play_pack( SDL_Surface *surface, const string &path );
void pack_done_screen( SDL_Surface *surface, const string &pack_name );

void level_screen( SDL_Surface *surface, const Level &level );
void level_loop( SDL_Surface *surface, Level &level );

enum
{
	USER_FRAME,
	USER_UPDATE
};

Uint32 push_frame_event( Uint32, void * );
Uint32 push_update_event( Uint32, void * );

int fps = 50, ups = 40;
int frame_ticks = 1000 / min(fps, ups),
    update_ticks = 1000 / ups;

//! in-game menu
//! best-time high score list
//! add particle class

map<int, Sprite> font_sprites;

bool global_quit = false;

int main( int argc, char *argv[] )
{
	if (SDL_Init(SDL_INIT_TIMER) == -1)
	{
		cerr << SDL_GetError() << endl;
		exit(EXIT_FAILURE);
	}
	SDL_Surface *surface = init_video();
	init_audio();
	
	controllers.push_back(new Keyboard());
	controllers.push_back(new Joystick(0));
	
	{
		SDL_Color color = { 100, 100, 100 };
		font_sprites[1] = Sprite(1, 1, color);
		
		font_sprites[3] = Sprite("img/font_gray_3.ppm");
		font_sprites[4] = Sprite("img/font_gray_4.ppm");
	}
	
	SDL_TimerID frame_timer = SDL_AddTimer(0, push_frame_event, NULL);
	SDL_TimerID update_timer = SDL_AddTimer(0, push_update_event, NULL);
	
	game_menu(surface);
	
	SDL_RemoveTimer(frame_timer);
	SDL_RemoveTimer(update_timer);
	
	for (vector<Controller *>::iterator i = controllers.begin(); i != controllers.end(); )
	{
		delete *i;
		controllers.erase(i);
	}
	
	deinit_audio();
	SDL_Quit();
	
	return EXIT_SUCCESS;
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
				
				font.blit(surface, screen_width / 2, screen_height / 2 - font.height(font_sprites[4]) * 2, "Bouncing", font_sprites[4], Font::center);
				font.blit(surface, screen_width / 2, screen_height / 2 - font.height(font_sprites[4]), "Frustration", font_sprites[4], Font::center);
				
				for (unsigned int i = 0; i < COUNTOF(menu_items); ++i)
				{
					int x = screen_width * ((Fixed)((signed)i + 1) / ((signed)COUNTOF(menu_items) + 1)),
						y = screen_height - font.height(font_sprites[3]) * 3;
					
					font.blit(surface, x, y, menu_items[i], font_sprites[3], Font::center, (i == selection) ? SDL_ALPHA_OPAQUE : 128);
				}
				
				font.blit(surface, 0, screen_height - font.height(font_sprites[1]), "v0.1 BETA", font_sprites[1], Font::left);
				
				font.blit(surface, screen_width - 1, screen_height - font.height(font_sprites[1]) * 2, "programming, graphics, and sound:", font_sprites[1], Font::right);
				font.blit(surface, screen_width - 1, screen_height - font.height(font_sprites[1]), "Carl Reinke (a.k.a. Mindless)", font_sprites[1], Font::right);
				
				SDL_Flip(surface);
				break;
				
			case USER_UPDATE:
				for (vector<Controller *>::iterator c = controllers.begin(); c != controllers.end(); ++c)
					(*c)->update();
				
				for (int i = 0; i < 4; ++i)
				{
					int x = screen_width * ((Fixed)((signed)selection + 1) / ((signed)COUNTOF(menu_items) + 1)),
						y = screen_height - font.height(font_sprites[3]) * 2;
					
					ball.update(ball.x > x ? -1 : 1);
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
	Pack_Entry( string name, string author, string path ) : name(name), author(author), path(path) {  }
	string name, author, path;
	bool operator<( const Pack_Entry &that ) const { return this->name < that.name; }
};

void pack_menu( SDL_Surface *surface )
{
	string path = "lvl/";
	
	vector<Pack_Entry> packs;
	
	// populate the pack list
	{
		DIR *dir = opendir(path.c_str());
		if (dir == NULL)
			return;
		
		struct dirent *dir_ent;
		while ((dir_ent = readdir(dir)) != NULL)
		{
			string filename = path + dir_ent->d_name + "/" + "meta";
			
			struct stat buffer;
			if (stat(filename.c_str(), &buffer) == 0)
			{
				string pack_name, author;
				
				ifstream pack_data(filename.c_str());
				getline(pack_data, pack_name);
				getline(pack_data, author);
				
				if (!pack_data.fail())
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
				path += packs[selection].path;
				play_pack(surface, path);
				
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
					for (vector<Controller *>::iterator c = controllers.begin(); c != controllers.end(); ++c)
						(*c)->update();
					
					level.update();
				}
				break;
			}
			break;
			
		default:
			break;
		}
	}
}

Uint32 push_frame_event( Uint32 interval, void * )
{
	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user.type = SDL_USEREVENT;
	event.user.code = USER_FRAME;
	
	SDL_PushEvent(&event);
	
	return frame_ticks;
}

Uint32 push_update_event( Uint32 interval, void * )
{
	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user.type = SDL_USEREVENT;
	event.user.code = USER_UPDATE;
	
	SDL_PushEvent(&event);
	
	return update_ticks;
}
