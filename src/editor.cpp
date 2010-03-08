#include "controller/controller.hpp"
#include "editor.hpp"
#include "game.hpp"
#include "main.hpp"
#include "game_menus.hpp"
#include "misc.hpp"
#include "sdl_ext.hpp"
#include "video/font.hpp"
#include "video/video.hpp"
#include "volume.hpp"

using namespace std;

vector<Sprite> Editor::block_sprites;
bitset<Block::_max> Editor::block_type_unusable;

Editor::Editor( void )
: cursor_block(Block::none),
  level("UNNAMED", align(screen_width, Block::width), align(screen_height, Block::height))
{
	if (block_sprites.empty())
	{
		block_sprites = Block(0, 0, Block::none).sprites;
		block_sprites[Block::none] = Sprite(Block::width, Block::height, SDL_Color_RGBA(0, 0, 0));
		block_sprites[Block::ball] = Sprite(sprite_directory + "editor/ball.ppm");
		block_sprites[Block::exit] = Sprite(sprite_directory + "editor/exit.ppm");
	}
	if (block_type_unusable.none())
	{
		block_type_unusable.set(Block::toggle_0_1);
		block_type_unusable.set(Block::toggle_1_0);
	}
	
	reset();
	
#ifndef TARGET_GP2X
	SDL_ShowCursor(SDL_ENABLE);
#endif
}

Editor::~Editor( void )
{
#ifndef TARGET_GP2X
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
		case Controller::start_key:
			menu();
			break;
		case Controller::quit_key:
			loop_quit = true;
			break;
			
		case Controller::right_key:
			cursor_x += Block::width;
			if (cursor_x < level.width)
			{
				x_offset = min(x_offset, screen_width - cursor_x - Block::width);
				break;
			}
			// if cursor outside level, undo movement
		case Controller::left_key:
			if (cursor_x > 0)
			{
				cursor_x -= Block::width;
				x_offset = max(x_offset, -cursor_x);
			}
			break;
		case Controller::down_key:
			cursor_y += Block::height;
			if (cursor_y < level.height)
			{
				y_offset = min(y_offset, screen_height - cursor_y - Block::height);
				break;
			}
			// if cursor outside level, undo movement
		case Controller::up_key:
			if (cursor_y > 0)
			{
				cursor_y -= Block::height;
				y_offset = max(y_offset, -cursor_y);
			}
			break;
			
		case Controller::select_key:
			set_block_at_position(cursor_x, cursor_y, static_cast<Block::Type>(cursor_block));
			break;
		case Controller::back_key:
			{
				vector<Block>::iterator block = block_at_position(cursor_x, cursor_y);
				cursor_block = (block == level.blocks.end()) ? Block::none : block->type;
			}
			break;
			
		case Controller::left_shoulder_key:
			do
			{
				if (cursor_block == 0)
					cursor_block = Block::_max;
				--cursor_block;
			}
			while (block_type_unusable[cursor_block]);
			break;
		case Controller::right_shoulder_key:
			do
			{
				++cursor_block %= Block::_max;
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
		
		cursor_x = -x_offset + align(e.motion.x, Block::width);
		cursor_y = -y_offset + align(e.motion.y, Block::height);
		break;
	case SDL_MOUSEBUTTONDOWN:
		e.button.y /= video_scale;
		e.button.x /= video_scale;
		
		switch (e.button.button)
		{
		case SDL_BUTTON_LEFT:
			set_block_at_position(-x_offset + e.button.x, -y_offset + e.button.y, static_cast<Block::Type>(cursor_block));
			break;
		case SDL_BUTTON_RIGHT:
			{
				vector<Block>::iterator block = block_at_position(-x_offset + e.button.x, -y_offset + e.button.y);
				cursor_block = (block == level.blocks.end()) ? Block::none : block->type;
			}
			break;
			
		case SDL_BUTTON_WHEELUP:
			do
			{
				if (cursor_block == 0)
					cursor_block = Block::_max;
				--cursor_block;
			}
			while (block_type_unusable[cursor_block]);
			break;
		case SDL_BUTTON_MIDDLE:
		case SDL_BUTTON_WHEELDOWN:
			do
			{
				++cursor_block %= Block::_max;
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
	BOOST_FOREACH (boost::shared_ptr<Controller> &controller, controllers)
		controller->update();
}

void Editor::menu( void )
{
	SimpleMenu menu(surface);
	const string entries[] =
	{
		"Play",
		"Save",
		"Save As",
		"Load",
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
				Game game(level);
				
				do
				{
					game.reset();
					game.loop(surface);
				}
				while (game.state == Game::lost || game.state == Game::restart);
			}
			break;
			
		case 1:  // Save
			level.validate();
			if (!save(level.path))
			{
		case 2:  // Save As
				LevelSetMenu level_set_menu(true);
				level_set_menu.loop(surface);
				if (level_set_menu.no_selection)
					break;  // cancel save
				
				LevelSet &level_set = level_set_menu.entries[level_set_menu.selection];
				if (level_set.invalid())
				{
					level_set.name = "UNNAMED";
					level_set.author = "NOBODY";
					
					TextEntryMenu text_entry_menu("New Set:", level_set.name);
					text_entry_menu.loop(surface);
					if (text_entry_menu.no_selection)
						break;  // cancel save
					
					level_set.name = text_entry_menu.text;
				}
				else
					level_set.load_levels();
				
				LevelMenu level_menu(level_set, true);
				level_menu.loop(surface);
				if (level_menu.no_selection)
					break;  // cancel save
				
				if (level_menu.selection < level_set.levels.size())
				{
					level.path = level_set.levels[level_menu.selection].path;
				}
				else  // save as new level
				{
					level.name = "UNNAMED";
					
					TextEntryMenu text_entry_menu("New Level:", level.name);
					text_entry_menu.loop(surface);
					if (text_entry_menu.no_selection)
						break;  // cancel save
					
					level.name = text_entry_menu.text;
					
					level_set.append_level(level);
					level_set.save_meta();
				}
				level.validate();
				save(level.path);
			}
			break;
			
		case 3:  // Load
			for (; ; )  // choose a level set
			{
				LevelSetMenu level_set_menu(false);
				level_set_menu.loop(surface);
				if (level_set_menu.no_selection)
					break;  // back to editor
				
				LevelSet &level_set = level_set_menu.entries[level_set_menu.selection];
				level_set.load_levels();
				
				LevelMenu level_menu(level_set, false);
				level_menu.loop(surface);
				if (level_menu.no_selection)
					continue;  // back to level set menu
				
				load(level_set.levels[level_menu.selection].path);
				
				break;  // back to editor
			}
			break;
			
		case 4:  // Quit
			loop_quit = true;
			break;
		}
	}
}

bool Editor::load( const string &level_data_path )
{
	bool temp = level.load(level_data_path);
	reset();
	
	return temp;
}

bool Editor::save( const string &level_data_path ) const
{
	bool success = level.save(level_data_path);
	
	// remove highscore replay because it is probably no longer valid
	if (success)
		unlink(level.get_score_path().c_str());
	
	return success;
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
	
	BOOST_FOREACH (const Block &block, level.blocks)
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
		sprite[i].blit(surface, x_offset + cursor_x - 1, y_offset + cursor_y + Block::height + 1 - sprite[i].height(), alpha);
		sprite[i].blit(surface, x_offset + cursor_x + Block::width + 1 - sprite[i].width(), y_offset + cursor_y - 1, alpha);
		sprite[i].blit(surface, x_offset + cursor_x + Block::width + 1 - sprite[i].width(), y_offset + cursor_y + Block::height + 1 - sprite[i].height(), alpha);
	}
	
	// currently held block preview
	{
		const int x = surface->w - (Block::width * 3) / 2,
		          y = (y_offset + cursor_y < screen_height * 3 / 4)  // if cursor on upper section of screen
		            ? (surface->h - (Block::height * 3) / 2)
		            : (Block::height / 2);
		
		Sprite(Block::width + 2, Block::width + 2, SDL_Color_RGBA(255, 255, 255)).blit(surface, x - 1, y - 1, alpha);
		block_sprites[cursor_block].blit(surface, x, y, alpha);
	}
	
	// cursor position message
	{
		const Sprite sprite(1, 1, SDL_Color_RGBA(255, 255, 255));
		
		const int x = font.width("N", sprite),
		          y = (y_offset + cursor_y < screen_height * 3 / 4)  // if cursor on upper section of screen
		            ? (surface->h - (font.height(sprite) * 3) / 2)
		            : (font.height(sprite) / 2);
		
		ostringstream buffer;
		buffer << (cursor_x / Block::width) << ", " << (cursor_y / Block::height);
		font.blit(surface, x, y, buffer.str(), sprite, Font::majuscule, Font::left, alpha);
	}
}

vector<Block>::iterator Editor::block_at_position( int x, int y )
{
	x -= x % Block::width;
	y -= y % Block::height;
	
	for (vector<Block>::reverse_iterator block = level.blocks.rbegin(); block != level.blocks.rend(); ++block)
		if (block->x == x && block->y == y)
			return block.base() - 1;
	
	return level.blocks.end();
}

void Editor::set_block_at_position( int x, int y, Block::Type type )
{
	x -= x % Block::width;
	y -= y % Block::height;
	
	vector<Block>::iterator block = block_at_position(x, y);
	
	if (type == Block::none)
	{
		if (block != level.blocks.end())
			level.blocks.erase(block);
	}
	else
	{
		if (block != level.blocks.end())
			*block = Block(x, y, type);
		else
			level.blocks.push_back(Block(x, y, type));
	}
}
