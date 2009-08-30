#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "sprite.hpp"
#include "SDL.h"

class Block
{
public:
	enum types
	{
		none = ' ',
		ball = '.',
		exit = 'x',
		normal = '=',
		bomb = '*',
		cracked = '-',
		boost_up = '^',
		boost_left = '<',
		boost_right = '>'
	};
	
	Block( int x, int y, types type );
	
	static const int height = 16, width = 16;
	
	void draw( SDL_Surface *, int x_offset, int y_offset, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
private:
	int x, y;
	
	types type;
	
	static std::map<types, Sprite> sprites;
	
	bool can_collide( void ) const;
	
	friend class Level;
};

#endif // BLOCK_HPP
