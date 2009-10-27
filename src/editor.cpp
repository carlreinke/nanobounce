#include "controller.hpp"
#include "editor.hpp"
#include "video.hpp"
#include "volume.hpp"
#include "sdl_ext.hpp"

using namespace std;

map<Block::types, Sprite> Editor::block_sprites;

Editor::Editor( void )
{
	if (block_sprites.empty())
	{
		block_sprites = Block(0, 0, Block::none).sprites;
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
		case SDLK_RETURN:
			// TODO: menu
			break;
		case SDLK_ESCAPE:
			loop_quit = true;
			break;
			
		case SDLK_RIGHT:
			cursor_x += Block::width;
			if (cursor_x < level.width)
				break;
		case SDLK_LEFT:
			if (cursor_x > 0)
				cursor_x -= Block::width;
			break;
		case SDLK_DOWN:
			cursor_y += Block::height;
			if (cursor_y < level.height)
				break;
		case SDLK_UP:
			if (cursor_y > 0)
				cursor_y -= Block::height;
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

bool Editor::load( const string &level_data_path )
{
	bool temp = level.load(level_data_path);
	reset();
	
	return temp;
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
	
	// TODO: replace this with a real cursor
	Sprite(3, 2, SDL_Color_RGBA(255, 255, 255)).blit(surface, cursor_x, cursor_y);
	Sprite(2, 3, SDL_Color_RGBA(  0,   0,   0)).blit(surface, cursor_x, cursor_y + Block::height - 3);
	Sprite(2, 3, SDL_Color_RGBA(  0,   0,   0)).blit(surface, cursor_x + Block::width - 2, cursor_y);
	Sprite(3, 2, SDL_Color_RGBA(255, 255, 255)).blit(surface, cursor_x + Block::width - 3, cursor_y + Block::height - 2);
}
