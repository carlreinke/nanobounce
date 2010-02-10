#include "block.hpp"
#include "main.hpp"
#include "video/sprite.hpp"

using namespace std;

const int Block::width = 16, Block::height = 16;

std::vector<Sprite> Block::sprites;

Block::Block( int x, int y, Type type )
: x(x), y(y), type(type)
{
	if (sprites.empty())
	{
		sprites.resize(_max);
		
		boost::array<pair<Sprite *, string>, 4 + 4 + 3 + 3> sprite_names =
		{{
			make_pair(&sprites[exit],    "exit"),
			make_pair(&sprites[normal],  "block"),
			make_pair(&sprites[nuke],    "nuke"),
			make_pair(&sprites[recycle], "recycle"),
			
			make_pair(&sprites[toggle_0],      "toggle_b"),
			make_pair(&sprites[toggle_0_star], "toggle_b_star"),
			make_pair(&sprites[toggle_1],      "toggle_y"),
			make_pair(&sprites[toggle_1_star], "toggle_y_star"),
			
			make_pair(&sprites[boost_up],    "boost_up"),
			make_pair(&sprites[boost_left],  "boost_left"),
			make_pair(&sprites[boost_right], "boost_right"),
			
			make_pair(&sprites[push_up],    "push_up"),
			make_pair(&sprites[push_left],  "push_left"),
			make_pair(&sprites[push_right], "push_right"),
		}};
		
		typedef pair<Sprite *, string> SpritePair;
		BOOST_FOREACH (const SpritePair &i, sprite_names)
			*i.first = Sprite(sprite_directory + i.second + ".ppm");
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
	case toggle_0:
		collidable = true;
		ignore = true;
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
