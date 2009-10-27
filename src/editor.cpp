#include "controller.hpp"
#include "editor.hpp"
#include "main.hpp"
#include "video.hpp"
#include "volume.hpp"

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
}
