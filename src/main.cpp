#include "audio.hpp"
#include "controller/controller.hpp"
#include "controller/joystick.hpp"
#include "controller/keyboard.hpp"
#include "editor.hpp"
#include "font.hpp"
#include "game.hpp"
#include "highscore.hpp"
#include "main.hpp"
#include "menu.hpp"
#include "video.hpp"

using namespace std;

bool global_quit = false;

const string level_directory = "levels/",
             music_directory = "music/",
             sample_directory = "samples/",
             sprite_directory = "sprites/",
             font_directory = sprite_directory + "fonts/";

int main( int argc, char *argv[] )
{
	bool editor = false, replay = false;
	
	int opt;
	while ((opt = getopt(argc, argv, "aemr:s")) != -1)
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
	
	cout << "Nanobounce" << endl
	     << "Copyright 2010 Carl Reinke" << endl
	     << "This program comes with ABSOLUTELY NO WARRANTY." << endl
	     << endl;
	
	if (SDL_Init(0) == -1)
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
	
	controllers.push_back(boost::shared_ptr<Controller>(new Keyboard()));
	controllers.push_back(boost::shared_ptr<Controller>(new Joystick(0)));
	
	if (editor)
	{
#ifndef TARGET_GP2X
		SDL_ShowCursor(SDL_ENABLE);
#endif
		
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
	
	controllers.clear();
	disabled_controllers.clear();
	
	deinit_audio();
	SDL_Quit();
	
	return EXIT_SUCCESS;
}
