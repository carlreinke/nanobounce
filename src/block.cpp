#include "block.hpp"
#include "main.hpp"
#include "sdl_ext.hpp"
#include "video/sprite.hpp"

const int Block::width = 16, Block::height = 16;

std::vector<Sprite> Block::sprites;

struct SpriteName { Block::Type type; std::string name; uint offset; };

Block::Block( int x, int y, Type type )
: x(x), y(y), type(type), initial_type(type)
{
	if (sprites.empty())
	{
		sprites.resize(Type_COUNT);
		
		boost::array<SpriteName, 4 + 3 + 3 + 3 + 3> sprite_names =
		{{
			{ EXIT,    "exit", 0 },
			{ NORMAL,  "block", 0 },
			{ NUKE,    "nuke", 0 },
			{ RECYCLE, "recycle", 0 },
			
			{ TOGGLE_0_0,    "toggle_b", 0 },
			{ TOGGLE_0_1,    "toggle_b", 1 },
			{ TOGGLE_0_STAR, "toggle_b", 2 },
			
			{ TOGGLE_1_0,    "toggle_y", 0 },
			{ TOGGLE_1_1,    "toggle_y", 1 },
			{ TOGGLE_1_STAR, "toggle_y", 2 },
			
			{ BOOST_UP,    "boost", 0 },
			{ BOOST_LEFT,  "boost", 1 },
			{ BOOST_RIGHT, "boost", 2 },
			
			{ PUSH_UP,    "push", 0 },
			{ PUSH_LEFT,  "push", 1 },
			{ PUSH_RIGHT, "push", 2 },
		}};
		
		for (const SpriteName &i : sprite_names)
			sprites[i.type] = Sprite(Sprite(sprite_directory + i.name + ".ppm"), SDL_RectXYWH(i.offset * width, 0, width, height));
	}
}

void Block::reset( void )
{
	type = initial_type;
	
	switch (type)
	{
	case NONE:
	case BALL:
		property = HIDDEN;
		break;
		
	case TOGGLE_0_0:
	case TOGGLE_1_0:
		property = IGNORED;
		break;
		
	case EXIT:
	case PUSH_UP:
	case PUSH_LEFT:
	case PUSH_RIGHT:
		property = TRIGGERABLE;
		break;
		
	default:
		property = COLLIDABLE;
		break;
	}
}

void Block::draw( SDL_Surface *surface, int x_offset, int y_offset, Uint8 alpha ) const
{
	if (property != HIDDEN)
		sprites[type].blit(surface, x_offset + x, y_offset + y, alpha);
}
