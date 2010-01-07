#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "sprite.hpp"
#include "SDL.h"

class Block
{
public:
	enum Type
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
		
		push_up,
		push_left,
		push_right,
		
		_max
	};
	
	Block( int x, int y, Type type );
	
	void reset( void );
	
	void draw( SDL_Surface *, int x_offset, int y_offset, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
	static const int height = 16, width = 16;
	
private:
	int x, y;
	
	Type type;
	
	bool collidable;
	bool ignore;
	
	static std::vector<Sprite> sprites;
	
	friend class Game;
	friend class Editor;
	friend class Level;
};

#endif // BLOCK_HPP
