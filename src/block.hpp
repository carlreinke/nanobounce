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
		
		toggle_0_0,
		toggle_0_1,
		toggle_0_star,
		
		toggle_1_0,
		toggle_1_1,
		toggle_1_star,
		
		boost_up,
		boost_left,
		boost_right,
		
		push_up,
		push_left,
		push_right,
		
		_max
	};
	
	enum Property
	{
		hidden,
		ignored,
		collidable,
		triggerable
	};
	
	Block( int x, int y, Type type );
	bool operator<( const Block &that ) const { return (y == that.y ? x < that.x : y < that.y); }
	
	void reset( void );
	
	void draw( SDL_Surface *, int x_offset, int y_offset, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
private:
	int x, y;
	static const int height, width;
	
	Type type, initial_type;
	Property property;
	
	static std::vector<Sprite> sprites;
	
	friend class Ball;
	friend class Editor;
	friend class Game;
	friend class Level;
	friend class Particle;
};

#endif // BLOCK_HPP
