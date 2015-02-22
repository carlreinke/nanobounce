#include "controller/controller.hpp"
#include "editor.hpp"
#include "file_system.hpp"
#include "game.hpp"
#include "main.hpp"
#include "game_menus.hpp"
#include "misc.hpp"
#include "sdl_ext.hpp"
#include "video/font.hpp"
#include "video/video.hpp"
#include "volume.hpp"

using std::max;
using std::min;

std::vector<Sprite> Editor::block_sprites;
std::bitset<LevelBlock::Type_COUNT> Editor::block_type_unusable;

Editor::Editor( void )
: cursor_block(LevelBlock::NONE),
  level()
{
	level.name = "UNNAMED";
	level.width = align(screen_width, LevelBlock::width);
	level.height = align(screen_height, LevelBlock::height);
	
	if (block_sprites.empty())
	{
		block_sprites = LevelBlock(0, 0, LevelBlock::NONE).sprites;
		block_sprites[LevelBlock::NONE] = Sprite(LevelBlock::width, LevelBlock::height, SDL_Color_RGBA(0, 0, 0));
		block_sprites[LevelBlock::BALL] = Sprite((sprite_directory / "editor" / "ball.ppm").string());
		block_sprites[LevelBlock::EXIT] = Sprite((sprite_directory / "editor" / "exit.ppm").string());
	}
	if (block_type_unusable.none())
	{
		block_type_unusable.set(LevelBlock::TOGGLE_0_1);
		block_type_unusable.set(LevelBlock::TOGGLE_1_0);
	}
	
	reset();
	
#ifdef HAS_MOUSE
	SDL_ShowCursor(SDL_ENABLE);
#endif
	
	load_level_temporary();
}

Editor::~Editor( void )
{
	save_level_temporary();
	
#ifdef HAS_MOUSE
	SDL_ShowCursor(SDL_DISABLE);
#endif
}

void Editor::handle_event( SDL_Event &e )
{
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case Controller::START_KEY:
			menu();
			break;
		case Controller::QUIT_KEY:
			loop_quit = true;
			break;
			
		case Controller::RIGHT_KEY:
			cursor_x += LevelBlock::width;
			if (cursor_x < level.width)
			{
				x_offset = min(x_offset, screen_width - cursor_x - LevelBlock::width);
				break;
			}
			// if cursor outside level, undo movement
		case Controller::LEFT_KEY:
			if (cursor_x > 0)
			{
				cursor_x -= LevelBlock::width;
				x_offset = max(x_offset, -cursor_x);
			}
			break;
		case Controller::DOWN_KEY:
			cursor_y += LevelBlock::height;
			if (cursor_y < level.height)
			{
				y_offset = min(y_offset, screen_height - cursor_y - LevelBlock::height);
				break;
			}
			// if cursor outside level, undo movement
		case Controller::UP_KEY:
			if (cursor_y > 0)
			{
				cursor_y -= LevelBlock::height;
				y_offset = max(y_offset, -cursor_y);
			}
			break;
			
		case Controller::SELECT_KEY:
			set_block_at_position(cursor_x, cursor_y, static_cast<LevelBlock::Type>(cursor_block));
			break;
		case Controller::BACK_KEY:
			{
				std::vector<LevelBlock>::iterator block = block_at_position(cursor_x, cursor_y);
				cursor_block = (block == level.blocks.end()) ? LevelBlock::NONE : block->type;
			}
			break;
			
		case Controller::LEFT_SHOULDER_KEY:
			do
			{
				if (cursor_block == 0)
					cursor_block = LevelBlock::Type_COUNT;
				--cursor_block;
			}
			while (block_type_unusable[cursor_block]);
			break;
		case Controller::RIGHT_SHOULDER_KEY:
			do
			{
				++cursor_block %= LevelBlock::Type_COUNT;
			}
			while (block_type_unusable[cursor_block]);
			break;
			
		default:
			break;
		}
		break;
		
	case SDL_MOUSEMOTION:
		e.motion.y /= video_scale;
		e.motion.x /= video_scale;
		
		cursor_x = -x_offset + align(e.motion.x, LevelBlock::width);
		cursor_y = -y_offset + align(e.motion.y, LevelBlock::height);
		break;
	case SDL_MOUSEBUTTONDOWN:
		e.button.y /= video_scale;
		e.button.x /= video_scale;
		
		switch (e.button.button)
		{
		case SDL_BUTTON_LEFT:
			set_block_at_position(-x_offset + e.button.x, -y_offset + e.button.y, static_cast<LevelBlock::Type>(cursor_block));
			break;
		case SDL_BUTTON_RIGHT:
			{
				std::vector<LevelBlock>::iterator block = block_at_position(-x_offset + e.button.x, -y_offset + e.button.y);
				cursor_block = (block == level.blocks.end()) ? LevelBlock::NONE : block->type;
			}
			break;
			
		case SDL_BUTTON_WHEELUP:
			do
			{
				if (cursor_block == 0)
					cursor_block = LevelBlock::Type_COUNT;
				--cursor_block;
			}
			while (block_type_unusable[cursor_block]);
			break;
		case SDL_BUTTON_MIDDLE:
		case SDL_BUTTON_WHEELDOWN:
			do
			{
				++cursor_block %= LevelBlock::Type_COUNT;
			}
			while (block_type_unusable[cursor_block]);
			break;
		}
		break;
		
	default:
		break;
	}
}

void Editor::update( void )
{
	// update controller
	for (std::shared_ptr<Controller> &controller : controllers)
		controller->update();
}

void Editor::menu( void )
{
	SimpleMenu menu(surface);
	const std::string entries[] =
	{
		"Play",
		"Save",
		"Save As",
		"Load",
		"Clear",
		"Quit",
	};
	for (uint i = 0; i < COUNTOF(entries); ++i)
		menu.entries.push_back(entries[i]);
	
	menu.loop(surface);
	
	if (!menu.no_selection)
	{
		switch (menu.selection)
		{
		case 0:  // Play
			{
				save_level_temporary();
				
				Game game(level);
				game.loop(surface);
			}
			break;
			
		case 1:  // Save
			level.normalize();
			
			if (!level_path.empty() && !save_level(level_path))
			{
		case 2:  // Save As
				LevelPackMenu level_pack_menu(false, true);
save_as_choose_level_pack:
				level_pack_menu.loop(surface);
				if (level_pack_menu.no_selection)
					break;  // cancel save

				LevelPack &level_pack = level_pack_menu.entries[level_pack_menu.selection].level_pack;
				if (level_pack_menu.selection == level_pack_menu.entry_count() - 1)  // new level pack
				{
					level_pack.name = "UNNAMED";
					level_pack.author = player_name;

					TextEntryMenu text_entry_menu("New Level Pack:", level_pack.name);
					text_entry_menu.loop(surface);
					if (text_entry_menu.no_selection)
						break;  // cancel save

					level_pack.name = text_entry_menu.text;
				}

				LevelMenu level_menu(level_pack, true);
save_as_choose_level:
				level_menu.loop(surface);
				if (level_menu.no_selection)
					goto save_as_choose_level_pack;

				if (level_menu.selection < level_pack.get_levels_count())
				{
					level.name = level_menu.entries[level_menu.selection];
				}
				else  // save as new level
				{
					level.name = "UNNAMED";

					TextEntryMenu text_entry_menu("New Level:", level.name);
					text_entry_menu.loop(surface);
					if (text_entry_menu.no_selection)
						goto save_as_choose_level;

					level.name = text_entry_menu.text;

					auto level_filename = level_pack.generate_level_filename(level);
					level_pack.level_filenames.push_back(level_filename);
					level_pack.save_meta();
				}

				level_path = level_pack.get_level_path(level_menu.selection);

				level.normalize();

				save_level(level_path);
			}
			break;
			
		case 3:  // Load
			{
				LevelPackMenu level_pack_menu(true, false);
load_choose_level_pack:
				level_pack_menu.loop(surface);
				if (level_pack_menu.no_selection)
					break;  // back to editor

				LevelPack &level_pack = level_pack_menu.entries[level_pack_menu.selection].level_pack;

				LevelMenu level_menu(level_pack, false);
				level_menu.loop(surface);
				if (level_menu.no_selection)
					goto load_choose_level_pack;

				load_level(level_pack.get_level_path(level_menu.selection));
			}
			break;
			
		case 4:  // Clear
			level_path.clear();
			
			level.name = "UNNAMED";
			level.blocks.clear();
			break;
			
		case 5:  // Quit
			loop_quit = true;
			break;
		}
	}
}

bool Editor::load_level( const boost::filesystem::path &path )
{
	bool success = level.load(path);
	
	if (success)
		level_path = path;
	
	reset();
	
	return success;
}

bool Editor::save_level( const boost::filesystem::path &path )
{
	bool success = level.save(path);
	
	if (success)
		level_path = path;
	
	// remove highscore replay because it is probably no longer valid
#if TODO
	if (success)
		unlink(level.get_score_path().c_str());
#endif
	
	return success;
}

bool Editor::load_level_temporary( void )
{
	std::ifstream stream((user_data_directory / "editor").string());
	
	Level new_level;
	new_level.load(stream);
	
	if (!new_level.invalid())
		level = new_level;
	
	return !new_level.invalid();
}

bool Editor::save_level_temporary( void ) const
{
	std::ofstream stream((user_data_directory / "editor").string());
	
	level.save(stream);
	
	stream.close();
	
#if defined(HAVE_SYNC)
	sync();
#endif
	
	return stream.good();
}

void Editor::reset( void )
{
	x_offset = 0;
	y_offset = 0;
	
	cursor_x = 0;
	cursor_y = 0;
}

void Editor::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	for (const LevelBlock &block : level.blocks)
		block_sprites[block.type].blit(surface, x_offset + block.x, y_offset + block.y, alpha);
	
	// cursor
	Sprite sprite[2] =
	{
		Sprite(4, 2, SDL_Color_RGBA(255, 255, 255)),
		Sprite(2, 4, SDL_Color_RGBA(255, 255, 255)),
	};
	
	for (unsigned int i = 0; i < COUNTOF(sprite); ++i)
	{
		sprite[i].blit(surface, x_offset + cursor_x - 1, y_offset + cursor_y - 1, alpha);
		sprite[i].blit(surface, x_offset + cursor_x - 1, y_offset + cursor_y + LevelBlock::height + 1 - sprite[i].height(), alpha);
		sprite[i].blit(surface, x_offset + cursor_x + LevelBlock::width + 1 - sprite[i].width(), y_offset + cursor_y - 1, alpha);
		sprite[i].blit(surface, x_offset + cursor_x + LevelBlock::width + 1 - sprite[i].width(), y_offset + cursor_y + LevelBlock::height + 1 - sprite[i].height(), alpha);
	}
	
	// currently held block preview
	{
		const int x = surface->w - (LevelBlock::width * 3) / 2,
		          y = (y_offset + cursor_y < screen_height * 3 / 4)  // if cursor on upper section of screen
		            ? (surface->h - (LevelBlock::height * 3) / 2)
		            : (LevelBlock::height / 2);
		
		Sprite(LevelBlock::width + 2, LevelBlock::width + 2, SDL_Color_RGBA(255, 255, 255)).blit(surface, x - 1, y - 1, alpha);
		block_sprites[cursor_block].blit(surface, x, y, alpha);
	}
	
	// cursor position message
	{
		const Sprite sprite(1, 1, SDL_Color_RGBA(255, 255, 255));
		
		const int x = font.width("N", sprite),
		          y = (y_offset + cursor_y < screen_height * 3 / 4)  // if cursor on upper section of screen
		            ? (surface->h - (font.height(sprite) * 3) / 2)
		            : (font.height(sprite) / 2);
		
		std::ostringstream buffer;
		buffer << (cursor_x / LevelBlock::width) << ", " << (cursor_y / LevelBlock::height);
		font.blit(surface, x, y, buffer.str(), sprite, Font::MAJUSCULE, Font::LEFT, alpha);
	}
}

std::vector<LevelBlock>::iterator Editor::block_at_position( int x, int y )
{
	x -= x % LevelBlock::width;
	y -= y % LevelBlock::height;
	
	for (auto block = level.blocks.rbegin(); block != level.blocks.rend(); ++block)
		if (block->x == x && block->y == y)
			return block.base() - 1;
	
	return level.blocks.end();
}

void Editor::set_block_at_position( int x, int y, LevelBlock::Type type )
{
	x -= x % LevelBlock::width;
	y -= y % LevelBlock::height;
	
	std::vector<LevelBlock>::iterator block = block_at_position(x, y);
	
	if (type == LevelBlock::NONE)
	{
		if (block != level.blocks.end())
			level.blocks.erase(block);
	}
	else
	{
		if (block != level.blocks.end())
			*block = LevelBlock(x, y, type);
		else
			level.blocks.push_back(LevelBlock(x, y, type));
	}
}
