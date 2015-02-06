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

using std::max;

bool global_quit = false;

const std::string level_directory = "levels/",
                  music_directory = "music/",
                  sample_directory = "samples/",
                  sprite_directory = "sprites/",
                  font_directory = sprite_directory + "fonts/";

std::string player_name = "NOBODY";

int main( int argc, char *argv[] )
{
	bool editor = false, replay = false;
	std::string level_path;
	
	int opt;
	while ((opt = getopt(argc, argv, "ae:mr:sv:")) != -1)
	{
		switch (opt)
		{
		case 'a':
			audio_disabled = true;
			
			std::cout << "audio disabled" << std::endl;
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
				disabled_controllers.push_back(std::make_shared<Replay>(score));
			}
			break;
			
		case 's':
			reverse_stereo = true;
			
			std::cout << "stereo reversal enabled" << std::endl;
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
		std::vector<std::string> music_filenames = read_directory_listing(music_directory);
		for (const std::string &filename : music_filenames)
			music_paths.push_back(music_directory + filename);
	}
	
	std::cout << "Nanobounce" << std::endl
	          << "Copyright 2010 Carl Reinke" << std::endl
	          << "This program comes with ABSOLUTELY NO WARRANTY." << std::endl
	          << std::endl;
	
	if (SDL_Init(0) == -1)
	{
		std::cerr << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}
	SDL_Surface *surface = init_video(false);
	init_audio();
	
	// read player name
	{
		std::ifstream name_file("player");
		getline(name_file, player_name);
	}
	
	Game::load_resources();
	
	font.load(font_directory + "font_04b21.pgm",
	          font_directory + "font_04b21.meta");
	
	font_sprites.resize(5);
	font_sprites[1] = Sprite(1, 1, SDL_Color_RGBA(128, 128, 128));
	font_sprites[2] = Sprite(font_directory + "font_gray_2.ppm");
	font_sprites[3] = Sprite(font_directory + "font_gray_3.ppm");
	font_sprites[4] = Sprite(font_directory + "font_gray_4.ppm");
	
#if defined(HAS_KEYBOARD)
	controllers.push_back(std::make_shared<Keyboard>());
#endif
	controllers.push_back(std::make_shared<Joystick>(0));
	
	if (editor)
	{
#if defined(HAS_MOUSE)
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
				for (LevelSetMenu set_menu; !global_quit; )  // choose a level set
				{
					set_menu.loop(surface);
					if (set_menu.no_selection)
						break;  // back to title menu
					
					LevelSet &level_set = set_menu.entries[set_menu.selection];
					level_set.load_levels();
					
					ScoredLevelMenu level_menu(level_set);
					level_menu.loop(surface);
					if (level_menu.no_selection)
						continue;  // back to level set menu
					
					if (Game::play(surface, std::make_pair(level_set.levels.begin() + level_menu.selection, level_set.levels.end())))
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
					const std::string menu_items[] =
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
						for (LevelSetMenu set_menu; !global_quit; )  // choose a level set
						{
							set_menu.loop(surface);
							if (set_menu.no_selection)
								break;  // back to title menu
							
							LevelSet &level_set = set_menu.entries[set_menu.selection];
							level_set.load_levels();
							
							for (ScoredLevelMenu level_menu(level_set, false, false); !global_quit; )  // choose a level
							{
								level_menu.loop(surface);
								if (level_menu.no_selection)
									break;  // back to level set menu
								
								Level &level = level_set.levels[level_menu.selection];
								
								bool restart = true;
								while (restart)
								{
									Game game(level, Controller::Set(1, std::make_shared<Replay>(Highscore(level.get_score_path()))));
									game.loop(surface);
									
									restart = (game.state == Game::restart);
								}
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
	
	// save player name
	{
		std::ofstream name_file("player");
		name_file << player_name;
	}
	
	controllers.clear();
	disabled_controllers.clear();
	
	// freeing surfaces after SDL_Quit() is disastrous on some platforms, so we free them here
	font_sprites.clear();
	Ball::static_destruction_clean_up();
	Block::static_destruction_clean_up();
	Editor::static_destruction_clean_up();
	
	deinit_audio();
	SDL_Quit();
	
	return EXIT_SUCCESS;
}
