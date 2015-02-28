/*  nanobounce/src/main.cpp
 *  
 *  Copyright 2015 Carl Reinke
 *  
 *  This program is non-commercial, open-source software; you can redistribute
 *  it and/or modify it under the terms of the MAME License as included along
 *  with this program.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  MAME License for more details.
 *  
 *  You should have received a copy of the MAME License along with this
 *  program; if not, see <http://www.intoorbit.org/licenses/MAME.txt>.
 */
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

#include <getopt.h>

using std::max;

bool global_quit = false;

const boost::filesystem::path level_directory = "levels";
const boost::filesystem::path music_directory = "music";
const boost::filesystem::path sample_directory = "samples";
const boost::filesystem::path sprite_directory = "sprites";
const boost::filesystem::path font_directory = sprite_directory / "fonts";

std::string player_name = "NAMELESS";

static void menu_title( SDL_Surface *surface );

int main( int argc, char *argv[] )
{
	bool fullscreen = false;
	bool editor = false;
	std::string level_path;
	
	int opt;
	while ((opt = getopt(argc, argv, "ae:fmr:sv:")) != -1)
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
			
		case 'f':
			fullscreen = true;
			break;
			
		case 'm':
			audio_mode = NO_MUSIC;
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
		auto music_filenames = list_files(music_directory);
		for (const std::string &filename : music_filenames)
			music_paths.push_back((music_directory / filename).string());
	}
	
	std::cout << "Nanobounce" << std::endl
	          << "Copyright 2015 Carl Reinke" << std::endl
	          << "This program comes with ABSOLUTELY NO WARRANTY." << std::endl
	          << std::endl;
	
	if (SDL_Init(0) == -1)
	{
		std::cerr << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}
	
	SDL_Surface *surface = init_video(fullscreen);
	
	init_audio();
	
	// read player name
	{
		std::ifstream name_file((user_data_directory / "player").string());
		std::getline(name_file, player_name);
	}
	
	Game::load_resources();
	
	font.load((font_directory / "font_04b21.pgm").string(),
	          (font_directory / "font_04b21.meta").string());
	
	font_sprites.resize(5);
	font_sprites[1] = Sprite(1, 1, SDL_Color_RGBA(128, 128, 128));
	font_sprites[2] = Sprite((font_directory / "font_gray_2.ppm").string());
	font_sprites[3] = Sprite((font_directory / "font_gray_3.ppm").string());
	font_sprites[4] = Sprite((font_directory / "font_gray_4.ppm").string());
	
	boost::filesystem::create_directories(user_data_directory);
	
	const auto controller_path = user_data_directory / "controller.conf";
	
	if (!boost::filesystem::exists(controller_path))
		boost::filesystem::copy("controller.conf", controller_path);
	
#if defined(HAS_KEYBOARD)
	controllers.push_back(std::make_shared<Keyboard>());
#endif
	
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
		controllers.push_back(std::make_shared<Joystick>(i));
	
	for (auto &controller : controllers)
	{
		auto *configurable_controller = dynamic_cast<ConfigurableController *>(controller.get());
		if (configurable_controller != nullptr)
			configurable_controller->load_controls_mapping(controller_path.string());
	}
	
	if (editor)
	{
#if defined(HAS_MOUSE)
		SDL_ShowCursor(SDL_ENABLE);
#endif
		
		Editor editor;
		editor.load_level(level_path);
		
		editor.loop(surface);
	}
	else
	{
		menu_title(surface);
	}
	
	// save player name
	{
		std::ofstream name_file((user_data_directory / "player").string());
		name_file << player_name;
	}
	
	controllers.clear();
	disabled_controllers.clear();
	
	// freeing surfaces after SDL_Quit() is disastrous on some platforms, so we free them here
	font_sprites.clear();
	Ball::static_destruction_clean_up();
	LevelBlock::static_destruction_clean_up();
	Editor::static_destruction_clean_up();
	
	deinit_audio();
	
	SDL_Quit();
	
	return EXIT_SUCCESS;
}

enum MenuResult
{
	QUIT,
	BACK,
	DONE,
};

static MenuResult menu_play( SDL_Surface *surface );

static MenuResult menu_more( SDL_Surface *surface );

static void menu_title( SDL_Surface *surface )
{
	// at this point, ticks should be a decent seed
	srand(SDL_GetTicks());

	while (!global_quit)
	{
		GameMenu menu;
		menu.loop(surface);

		if (menu.no_selection)
			menu.selection = 2; // choose quit

		switch (menu.selection)
		{
		case 0:  // Play
			menu_play(surface);
			break;

		case 1:  // More
			menu_more(surface);
			break;

		case 2:  // Quit
			global_quit = true;
			break;
		}
	}
}

static MenuResult menu_play( SDL_Surface *surface, LevelPack &level_pack );

static MenuResult menu_play( SDL_Surface *surface )
{
	LevelPackMenu pack_selector;
	
again:
	if (global_quit)
		return QUIT;
	
	pack_selector.loop(surface);
	if (pack_selector.no_selection)
		return BACK;
	
	LevelPack &level_pack = pack_selector.entries[pack_selector.selection].level_pack;
	
	switch (menu_play(surface, level_pack))
	{
		case QUIT:
			return QUIT;
		case BACK:
			goto again;
		case DONE:
			return DONE;
	}
	
	assert(false);
	return DONE;
}

static MenuResult menu_play( SDL_Surface *surface, LevelPack &level_pack, uint i );

static MenuResult menu_play( SDL_Surface *surface, LevelPack &level_pack )
{
	ScoredLevelMenu level_selector(level_pack);
	
again:
	if (global_quit)
		return QUIT;
	
	level_selector.loop(surface);
	if (level_selector.no_selection)
		return BACK;
	
	for (uint i = level_selector.selection; i < level_pack.get_levels_count(); ++i)
	{
		if (global_quit)
			return QUIT;

		switch (menu_play(surface, level_pack, i))
		{
			case QUIT:
				return QUIT;
			case BACK:
				goto again;
			case DONE:
				break;
		}
	}
	
	LevelPackCongratsLoop congrats(level_pack);
	congrats.loop(surface);

	return BACK;
}

static MenuResult menu_play( SDL_Surface *surface, LevelPack &level_pack, uint i )
{
	Level level;

	auto level_path = level_pack.get_level_path(i);
	auto score_path = level_pack.get_score_path(i);

	if (!level.load(level_path))
		return DONE;

	LevelIntroLoop level_intro(level);
	level_intro.loop(surface);

	bool persistent_restart_selection = false;

	Game game(level);
	
again:
	if (global_quit)
		return QUIT;

	game.loop(surface);

	switch (game.state)
	{
		case Game::NONE:
			goto again;
		
		case Game::WON:
		{
			Highscore best_highscore;

			best_highscore.load(score_path);

			if (game.highscore.get_time_ms() < best_highscore.get_time_ms() || best_highscore.invalid())
			{
				game.highscore.set_player_name(player_name);

				LevelWonBestTimeLoop menu(level, game.highscore);
				menu.loop(surface);

				if (!menu.no_selection)
					game.highscore.set_player_name(player_name = menu.text);

				game.highscore.save(score_path);
			}
			else
			{
				LevelWonLoop menu(level, best_highscore, game.highscore);
				if (persistent_restart_selection)
					menu.selection = 1;
				menu.loop(surface);
				if (menu.no_selection)
					menu.selection = 0;  // no retry

				switch (menu.selection)
				{
					case 0: // Next
						break;
					case 1: // Retry
						persistent_restart_selection = true;
						goto again;
				}
			}
			
			return DONE;
		}
		
		case Game::CHEAT_WON:
			return DONE;

		case Game::LOST:
			goto again;
			
		case Game::RESTART:
			goto again;
			
		case Game::QUIT:
			return QUIT;
	}
	
	assert(false);
	return DONE;
}

static MenuResult menu_watch_replays( SDL_Surface *surface );

static MenuResult menu_more( SDL_Surface *surface )
{
	SimpleMenu more_menu;
	const std::string menu_items[] =
	{
		"Watch Replays",
		"Make Levels",
		"Back"
	};
	for (uint i = 0; i < COUNTOF(menu_items); ++i)
		more_menu.entries.push_back(menu_items[i]);

	more_menu.loop(surface);
	if (more_menu.no_selection)
		return BACK;

	switch (more_menu.selection)
	{
	case 0:  // Watch Replays
		return menu_watch_replays(surface);
		
	case 1:  // Make Levels
		{
			Editor editor;
			editor.loop(surface);
		}
		return QUIT;
		
	case 2:  // Back
		return BACK;
	}
	
	assert(false);
	return DONE;
}

static MenuResult menu_watch_replays( SDL_Surface *surface, LevelPack &level_pack );

static MenuResult menu_watch_replays( SDL_Surface *surface )
{
	LevelPackMenu pack_selector;
	
again:
	if (global_quit)
		return QUIT;
	
	pack_selector.loop(surface);
	if (pack_selector.no_selection)
		return BACK;

	LevelPack &level_pack = pack_selector.entries[pack_selector.selection].level_pack;
	
	switch (menu_watch_replays(surface, level_pack))
	{
		case QUIT:
			return QUIT;
		case BACK:
			goto again;
		case DONE:
			return DONE;
	}
	
	assert(false);
	return DONE;
}

static MenuResult menu_watch_replays( SDL_Surface *surface, LevelPack &level_pack, uint i );

static MenuResult menu_watch_replays( SDL_Surface *surface, LevelPack &level_pack )
{
	ScoredLevelMenu level_selector(level_pack, false, false);

again:	
	if (global_quit)
		return QUIT;
	
	level_selector.loop(surface);
	if (level_selector.no_selection)
		return BACK;

	switch (menu_watch_replays(surface, level_pack, level_selector.selection))
	{
		case QUIT:
			goto again;
		case BACK:
			goto again;
		case DONE:
			goto again;
	}

	assert(false);
	return DONE;	
}

static MenuResult menu_watch_replays( SDL_Surface *surface, LevelPack &level_pack, uint i )
{
	Level level;
	Highscore score;

	auto level_path = level_pack.get_level_path(i);
	auto score_path = level_pack.get_score_path(i);

	if (!level.load(level_path))
		return DONE;

	if (!score.load(score_path))
		return DONE;
	
again:
	if (global_quit)
		return QUIT;
	
	Game game(level, Controllers(1, std::make_shared<Replay>(score)));
	game.loop(surface);

	switch (game.state)
	{
		case Game::NONE:
			assert(false);
			return BACK;
			
		case Game::WON:
			return DONE;
			
		case Game::CHEAT_WON:
			return DONE;
			
		case Game::LOST:
			return DONE;
			
		case Game::RESTART:
			goto again;
			
		case Game::QUIT:
			return QUIT;
	}
	
	assert(false);
	return DONE;
}
