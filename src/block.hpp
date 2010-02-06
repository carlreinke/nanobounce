#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "video/sprite.hpp"
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
		
		locked,
		locked_star,
		
		boost_up,
		boost_left,
		boost_right,
		
		push_up,
		push_left,
		push_right,
		
		_max
	};
	
	Block( int x, int y, Type type );
	bool operator<( const Block &that ) const { return (y == that.y ? x < that.x : y < that.y); }
	
	void reset( void );
	
	void draw( SDL_Surface *, int x_offset, int y_offset, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
private:
	int x, y;
	static const int height, width;
	
	Type type;
	
	bool collidable;
	bool ignore;
	
	static std::vector<Sprite> sprites;
	
	friend class Ball;
	friend class Editor;
	friend class Game;
	friend class Level;
};

#endif // BLOCK_HPP
