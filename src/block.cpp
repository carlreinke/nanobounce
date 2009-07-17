#include "block.h"
#include "sprite.h"

using namespace std;

map<Block::types, Sprite> Block::sprites;

Block::Block( int x, int y, types type )
: x(x), y(y), type(type)
{
	if (sprites.empty())
	{
		sprites[exit] = Sprite("img/blue.ppm");
		sprites[normal] = Sprite("img/gray.ppm");
		sprites[bomb] = Sprite("img/red.ppm");
		sprites[cracked] = Sprite("img/dark_gray.ppm");
		sprites[boost_up] = Sprite("img/green_up.ppm");
		sprites[boost_left] = Sprite("img/green_left.ppm");
		sprites[boost_right] = Sprite("img/green_right.ppm");
	}
}

void Block::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	sprites[type].blit(surface, x, y, alpha);
}

bool Block::can_collide( void ) const
{
	switch (type)
	{
	case none:
	case ball:
	case exit:
		return false;
		break;
	default:
		break;
	}
	return true;
}
