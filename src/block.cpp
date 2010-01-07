#include "block.hpp"
#include "sprite.hpp"

using namespace std;

std::vector<Sprite> Block::sprites;

Block::Block( int x, int y, Type type )
: x(x), y(y), type(type)
{
	if (sprites.empty())
	{
		sprites.resize(_max);
		sprites[exit] = Sprite("sprites/exit.ppm");
		sprites[normal] = Sprite("sprites/block.ppm");
		sprites[nuke] = Sprite("sprites/nuke.ppm");
		sprites[recycle] = Sprite("sprites/recycle.ppm");
		
		sprites[boost_up] = Sprite("sprites/boost_up.ppm");
		sprites[boost_left] = Sprite("sprites/boost_left.ppm");
		sprites[boost_right] = Sprite("sprites/boost_right.ppm");
		
		sprites[push_up] = Sprite("sprites/push_up.ppm");
		sprites[push_left] = Sprite("sprites/push_left.ppm");
		sprites[push_right] = Sprite("sprites/push_right.ppm");
	}
}

void Block::reset( void )
{
	switch (type)
	{
	case none:
	case ball:
		ignore = true;
		collidable = false;
		break;
	case exit:
	case push_up:
	case push_left:
	case push_right:
		collidable = false;
		ignore = false;
		break;
	default:
		collidable = true;
		ignore = false;
		break;
	}
}

void Block::draw( SDL_Surface *surface, int x_offset, int y_offset, Uint8 alpha ) const
{
	if (!ignore)
		sprites[type].blit(surface, x_offset + x, y_offset + y, alpha);
}
