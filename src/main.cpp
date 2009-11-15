#include "audio.hpp"
#include "controller.hpp"
#include "game.hpp"
#include "highscore.hpp"
#include "main.hpp"
#include "menu.hpp"
#include "video.hpp"

using namespace std;

Uint32 push_frame_event( Uint32, void * );
Uint32 push_update_event( Uint32, void * );

int fps = 50, ups = 40;
int ms_per_frame = 1000 / min(fps, ups),
    ms_per_update = 1000 / ups;

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
	
	{
		font_sprites[1] = Sprite(1, 1, SDL_Color_RGBA(100, 100, 100));
		font_sprites[2] = Sprite("sprites/fonts/font_gray_2.ppm");
		font_sprites[3] = Sprite("sprites/fonts/font_gray_3.ppm");
		font_sprites[4] = Sprite("sprites/fonts/font_gray_4.ppm");
	}
	
	SDL_TimerID frame_timer = SDL_AddTimer(0, push_frame_event, NULL);
	SDL_TimerID update_timer = SDL_AddTimer(0, push_update_event, NULL);
	
	bool replay = false;
	
	int opt;
	while ((opt = getopt(argc, argv, "r:")) != -1)
	{
		switch (opt)
		{
		case 'r':
			replay = true;
			
			controllers.push_back(new Replay(optarg));
			break;
		case '?':
		default:
			exit(EXIT_FAILURE);
			break;
		}
	} 
	
	if (replay && optind < argc)
	{
		Game game;
		game.load(argv[optind]);
		
		game.loop(surface);
	}
	else
	{
		controllers.push_back(new Keyboard());
		controllers.push_back(new Joystick(0));
		
		game_menu(surface);
	}
	
	SDL_RemoveTimer(frame_timer);
	SDL_RemoveTimer(update_timer);
	
	for (vector<Controller *>::iterator i = controllers.begin(); i != controllers.end(); )
	{
		delete *i;
		i = controllers.erase(i);
	}
	
	deinit_audio();
	SDL_Quit();
	
	return EXIT_SUCCESS;
}

Uint32 push_frame_event( Uint32, void * )
{
	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user.type = SDL_USEREVENT;
	event.user.code = USER_FRAME;
	
	SDL_PushEvent(&event);
	
	return ms_per_frame;
}

Uint32 push_update_event( Uint32, void * )
{
	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user.type = SDL_USEREVENT;
	event.user.code = USER_UPDATE;
	
	SDL_PushEvent(&event);
	
	return ms_per_update;
}
