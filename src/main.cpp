#include "audio/audio.hpp"
#include "controller/controller.hpp"
#include "controller/joystick.hpp"
#include "controller/keyboard.hpp"
#include "editor.hpp"
#include "file_system.hpp"
#include "game.hpp"
#include "game_loops.hpp"
#include "game_menus.hpp"
#include "highscore.hpp"
#include "main.hpp"
#include "video/font.hpp"
#include "video/video.hpp"

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
	string level_path;
	
	int opt;
	while ((opt = getopt(argc, argv, "ae:mr:sv:")) != -1)
	{
		switch (opt)
		{
		case 'a':
			audio_disabled = true;
			
			cout << "audio disabled" << endl;
			break;
			
		case 'e':
			editor = true;
			
			level_path = optarg;
			break;
			
		case 'm':
			audio_mode = NO_MUSIC;
			break;
			
		case 'r':
			replay = true;
			
			{
				const Highscore score(optarg);
				level_path = score.level_path;
				disabled_controllers.push_back(boost::shared_ptr<Controller>(new Replay(score)));
			}
			break;
			
		case 's':
			reverse_stereo = true;
			
			cout << "stereo reversal enabled" << endl;
			break;
			
		case 'v':
			video_scale = max(1, atoi(optarg));
			break;
			
		case '?':
		default:
			exit(EXIT_FAILURE);
			break;
		}
	} 
	
	{
		vector<string> music_filenames = directory_listing(music_directory);
		BOOST_FOREACH (const string &filename, music_filenames)
			music_paths.push_back(music_directory + filename);
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
	SDL_Surface *surface = init_video(false);
	init_audio();
	
	Game::load_resources();
	
	font.load(font_directory + "font_04b21.pgm",
	          font_directory + "font_04b21.meta");
	
	font_sprites.resize(5);
	font_sprites[1] = Sprite(1, 1, SDL_Color_RGBA(128, 128, 128));
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
		editor.load(level_path);
		
		editor.loop(surface);
	}
	else if (replay)
	{
		disabled_controllers.swap(controllers);
		
		Game game;
		game.load(level_path);
		
		game.loop(surface);
		
		disabled_controllers.swap(controllers);
	}
	else
	{
		while (!global_quit)
		{
			GameMenu menu;
			menu.loop(surface);
			
			// at this point, ticks should be a decent seed
			srand(SDL_GetTicks());
			
			if (menu.no_selection)
				menu.selection = 2; // choose quit
			
			switch (menu.selection)
			{
			case 0:  // Play
				for (; ; )  // choose a level set
				{
					LevelSetMenu set_menu;
					set_menu.loop(surface);
					if (set_menu.no_selection)
						break;  // back to title menu
					
					LevelSet &level_set = set_menu.entries[set_menu.selection];
					level_set.load_levels();
					
					ScoredLevelMenu level_menu(level_set);
					level_menu.loop(surface);
					if (level_menu.no_selection)
						continue;  // back to level set menu
					
					if (Game::play(surface, make_pair(level_set.levels.begin() + level_menu.selection, level_set.levels.end())))
					{
						LevelSetCongratsLoop congrats(level_set);
						congrats.loop(surface);
					}
					
					break;  // back to title menu
				}
				break;
				
			case 1:  // More
				{
					SimpleMenu more_menu;
					const string menu_items[] =
					{
						"Edit Levels",
						"View Replays",
						"Back"
					};
					for (uint i = 0; i < COUNTOF(menu_items); ++i)
						more_menu.entries.push_back(menu_items[i]);
					
					more_menu.loop(surface);
					if (more_menu.no_selection)
						break;  // back to title menu
					
					switch (more_menu.selection)
					{
					case 0:
						{
							Editor editor;
							editor.loop(surface);
						}
						break;
					case 1:
						for (; ; )  // choose a level set
						{
							LevelSetMenu set_menu;
							set_menu.loop(surface);
							if (set_menu.no_selection)
								break;  // back to title menu
							
							LevelSet &level_set = set_menu.entries[set_menu.selection];
							level_set.load_levels();
							
							for (; ; )  // choose a level
							{
								ScoredLevelMenu level_menu(level_set, false, false);
								level_menu.loop(surface);
								if (level_menu.no_selection)
									break;  // back to level set menu
								
								Level &level = level_set.levels[level_menu.selection];
								
								disabled_controllers.push_back(boost::shared_ptr<Controller>(new Replay(level.get_score_path())));
								disabled_controllers.swap(controllers);
								
								Game game(level);
								game.loop(surface);
								
								disabled_controllers.swap(controllers);
								disabled_controllers.clear();
							}
						}
						break;
					}
				}
				break;
				
			case 2:  // Quit
				global_quit = true;
				break;
			}
		}
	}
	
	controllers.clear();
	disabled_controllers.clear();
	
	deinit_audio();
	SDL_Quit();
	
	return EXIT_SUCCESS;
}
