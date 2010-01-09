#include "audio.hpp"
#include "controller.hpp"
#include "editor.hpp"
#include "font.hpp"
#include "game.hpp"
#include "highscore.hpp"
#include "main.hpp"
#include "menu.hpp"
#include "video.hpp"

using namespace std;

Uint32 push_frame_event( Uint32, void * );
Uint32 push_update_event( Uint32, void * );

bool global_quit = false;

const int fps = 40,
          ups = 40, ups_multiplier = 4;
const int ms_per_frame = 1000 / std::min(fps, ups),
          ms_per_update = 1000 / ups;

const string level_directory = "levels/",
             music_directory = "music/",
             sample_directory = "samples/",
             sprite_directory = "sprites/",
             font_directory = sprite_directory + "fonts/";

int main( int argc, char *argv[] )
{
	bool editor = false, replay = false;
	
	int opt;
	while ((opt = getopt(argc, argv, "ae:mr:s")) != -1)
	{
		switch (opt)
		{
		case 'a':
			audio_disabled = true;
			
			cout << "audio disabled" << endl;
			break;
			
		case 'e':
			editor = true;
			break;
			
		case 'm':
			audio_mode = NO_MUSIC;
			break;
			
		case 'r':
			replay = true;
			
			disabled_controllers.push_back(boost::shared_ptr<Controller>(new Replay(optarg)));
			break;
			
		case 's':
			reverse_stereo = true;
			
			cout << "stereo reversal enabled" << endl;
			break;
			
		case '?':
		default:
			exit(EXIT_FAILURE);
			break;
		}
	} 
	
	if (optind >= argc)
	{
		editor = false;
		replay = false;
	}
	
	if (SDL_Init(SDL_INIT_TIMER) == -1)
	{
		cerr << SDL_GetError() << endl;
		exit(EXIT_FAILURE);
	}
	SDL_Surface *surface = init_video();
	init_audio();
	
	font.load(font_directory + "font_04b21.pgm",
	          font_directory + "font_04b21.meta");
	
	font_sprites.resize(5);
	font_sprites[1] = Sprite(1, 1, SDL_Color_RGBA(100, 100, 100));
	font_sprites[2] = Sprite(font_directory + "font_gray_2.ppm");
	font_sprites[3] = Sprite(font_directory + "font_gray_3.ppm");
	font_sprites[4] = Sprite(font_directory + "font_gray_4.ppm");
	
	SDL_TimerID frame_timer = SDL_AddTimer(0, push_frame_event, NULL);
	SDL_TimerID update_timer = SDL_AddTimer(0, push_update_event, NULL);
	
	controllers.push_back(boost::shared_ptr<Controller>(new Keyboard()));
	controllers.push_back(boost::shared_ptr<Controller>(new Joystick(0)));
	
	if (editor)
	{
		Editor editor;
		editor.load(argv[optind]);
		
		editor.loop(surface);
	}
	else if (replay)
	{
		disabled_controllers.swap(controllers);
		
		Game game;
		game.load(argv[optind]);
		
		game.loop(surface);
		
		disabled_controllers.swap(controllers);
	}
	else
	{
		GameMenu menu;
		menu.loop(surface);
	}
	
	SDL_RemoveTimer(frame_timer);
	SDL_RemoveTimer(update_timer);
	
	controllers.clear();
	disabled_controllers.clear();
	
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
