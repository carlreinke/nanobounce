#include "controller.hpp"
#include "editor.hpp"
#include "font.hpp"
#include "game.hpp"
#include "menu.hpp"
#include "misc.hpp"
#include "video.hpp"
#include "volume.hpp"
#include "sdl_ext.hpp"

using namespace std;

vector<Sprite> Editor::block_sprites;

Editor::Editor( void )
: cursor_block(Block::none)
{
	if (block_sprites.empty())
	{
		block_sprites = Block(0, 0, Block::none).sprites;
		block_sprites[Block::none] = Sprite(Block::width, Block::height, SDL_Color_RGBA(0, 0, 0));
		block_sprites[Block::ball] = Sprite("sprites/editor/ball.ppm");
		block_sprites[Block::exit] = Sprite("sprites/editor/exit.ppm");
	}
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
			set_block_at_position(cursor_x, cursor_y, static_cast<Block::types>(cursor_block));
			break;
			
		case Controller::left_shoulder_key:
			if (cursor_block == 0)
				cursor_block = Block::_max;
			--cursor_block;
			break;
			
		case Controller::right_shoulder_key:
		case Controller::back_key:
			++cursor_block %= Block::_max;
			break;
			
		default:
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
	for (vector<Controller *>::iterator c = controllers.begin(); c != controllers.end(); ++c)
		(*c)->update();
}

void Editor::menu( void )
{
	SimpleMenu menu;
	const string entries[] =
	{
		"Play",
		"Save",
		"Continue",
		"Quit",
	};
	for (uint i = 0; i < COUNTOF(entries); ++i)
		menu.entries.push_back(entries[i]);
	
	menu.loop(SDL_GetVideoSurface());
	
	if (!menu.no_selection)
	{
		switch (menu.selection)
		{
		case 0:
			{
				Game game(level);
				game.loop(SDL_GetVideoSurface());
			}
			break;
		case 1:
			save("temp"); // TODO
			break;
		case 2:
			break;
		case 3:
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
	return level.save(level_data_path);
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
	
	for (vector<Block>::const_iterator block = level.blocks.begin(); block != level.blocks.end(); ++block)
		block_sprites[block->type].blit(surface, x_offset + block->x, y_offset + block->y, alpha);
	
	// cursor
	// TODO: replace this with a sprite cursor
	Sprite sprite[2] =
	{
		Sprite(4, 2, SDL_Color_RGBA(255, 255, 255)),
		Sprite(2, 4, SDL_Color_RGBA(255, 255, 255)),
	};
	
	for (unsigned int i = 0; i < COUNTOF(sprite); ++i)
	{
		sprite[i].blit(surface, x_offset + cursor_x - 1, y_offset + cursor_y - 1);
		sprite[i].blit(surface, x_offset + cursor_x - 1, y_offset + cursor_y + Block::height + 1 - sprite[i].height());
		sprite[i].blit(surface, x_offset + cursor_x + Block::width + 1 - sprite[i].width(), y_offset + cursor_y - 1);
		sprite[i].blit(surface, x_offset + cursor_x + Block::width + 1 - sprite[i].width(), y_offset + cursor_y + Block::height + 1 - sprite[i].height());
	}
	
	// currently held block preview
	{
		const int x = surface->w - (Block::width * 3) / 2,
		          y = (y_offset + cursor_y < screen_height * 3 / 4)  // if cursor on upper section of screen
		            ? (surface->h - (Block::height * 3) / 2)
		            : (Block::height / 2);
		
		Sprite(Block::width + 2, Block::width + 2, SDL_Color_RGBA(255, 255, 255)).blit(surface, x - 1, y - 1);
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

void Editor::set_block_at_position( int x, int y, Block::types type )
{
	x -= x % Block::width;
	y -= y % Block::height;
	
	for (vector<Block>::iterator block = level.blocks.begin(); block != level.blocks.end(); ++block)
	{
		if (block->x == x && block->y == y)
		{
			if (type == Block::none)
				level.blocks.erase(block);
			else
				block->type = type;
			return;
		}
	}
	
	level.blocks.push_back(Block(x, y, type));
}
