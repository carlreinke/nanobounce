#include "block.hpp"
#include "main.hpp"
#include "sdl_ext.hpp"
#include "video/sprite.hpp"

using namespace std;

const int Block::width = 16, Block::height = 16;

std::vector<Sprite> Block::sprites;

struct SpriteName { Block::Type type; string name; uint offset; };

Block::Block( int x, int y, Type type )
: x(x), y(y), type(type), initial_type(type)
{
	if (sprites.empty())
	{
		sprites.resize(_max);
		
		boost::array<SpriteName, 4 + 3 + 3 + 3 + 3> sprite_names =
		{{
			{ exit,    "exit", 0 },
			{ normal,  "block", 0 },
			{ nuke,    "nuke", 0 },
			{ recycle, "recycle", 0 },
			
			{ toggle_0_0,    "toggle_b", 0 },
			{ toggle_0_1,    "toggle_b", 1 },
			{ toggle_0_star, "toggle_b", 2 },
			
			{ toggle_1_0,    "toggle_y", 0 },
			{ toggle_1_1,    "toggle_y", 1 },
			{ toggle_1_star, "toggle_y", 2 },
			
			{ boost_up,    "boost", 0 },
			{ boost_left,  "boost", 1 },
			{ boost_right, "boost", 2 },
			
			{ push_up,    "push", 0 },
			{ push_left,  "push", 1 },
			{ push_right, "push", 2 },
		}};
		
		BOOST_FOREACH (const SpriteName &i, sprite_names)
			sprites[i.type] = Sprite(Sprite(sprite_directory + i.name + ".ppm"), SDL_RectXYWH(i.offset * width, 0, width, height));
	}
}

void Block::reset( void )
{
	type = initial_type;
	
	switch (type)
	{
	case none:
	case ball:
		property = hidden;
		break;
		
	case toggle_0_0:
	case toggle_1_0:
		property = ignored;
		break;
		
	case exit:
	case push_up:
	case push_left:
	case push_right:
		property = triggerable;
		break;
		
	default:
		property = collidable;
		break;
	}
}

void Block::draw( SDL_Surface *surface, int x_offset, int y_offset, Uint8 alpha ) const
{
	if (property != hidden)
		sprites[type].blit(surface, x_offset + x, y_offset + y, alpha);
}
