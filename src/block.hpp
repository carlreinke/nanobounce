#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "sprite.hpp"
#include "SDL.h"

class Block
{
public:
	enum types
	{
		none,
		ball,
		exit,
		normal,
		nuke,
		recycle,
		boost_up,
		boost_left,
		boost_right,
		_max
	};
	
	Block( int x, int y, types type );
	
	void reset( void );
	
	void draw( SDL_Surface *, int x_offset, int y_offset, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
	static const int height = 16, width = 16;
	
private:
	int x, y;
	
	types type;
	
	bool collideable;
	bool ignore;
	
	static std::vector<Sprite> sprites;
	
	friend class Game;
	friend class Editor;
};

#endif // BLOCK_HPP
