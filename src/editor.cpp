#include "editor.hpp"

using namespace std;

map<Block::types, Sprite> Editor::block_sprites;

Editor::Editor( void )
{
	if (block_sprites.empty())
	{
		block_sprites = Block::sprites;
		block_sprites[Block::ball] = Sprite("sprites/editor-ball.ppm");
		block_sprites[Block::exit] = Sprite("sprites/editor-exit.ppm");
	}
}

void Editor::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	for (vector<Block>::const_iterator block = level.blocks.begin(); block != level.blocks.end(); ++block)
		block_sprites[block->type].blit(surface, x_offset, y_offset, alpha);
}
