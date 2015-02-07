#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "video/sprite.hpp"

#include <SDL.h>

class Block
{
public:
	enum Type
	{
		NONE,
		BALL,
		EXIT,
		
		NORMAL,
		NUKE,
		RECYCLE,
		
		TOGGLE_0_0,
		TOGGLE_0_1,
		TOGGLE_0_STAR,
		
		TOGGLE_1_0,
		TOGGLE_1_1,
		TOGGLE_1_STAR,
		
		BOOST_UP,
		BOOST_LEFT,
		BOOST_RIGHT,
		
		PUSH_UP,
		PUSH_LEFT,
		PUSH_RIGHT,
		
		Type_COUNT
	};
	
	enum Property
	{
		HIDDEN,
		IGNORED,
		COLLIDABLE,
		TRIGGERABLE
	};
	
	Block( int x, int y, Type type );
	bool operator<( const Block &that ) const { return (y == that.y ? x < that.x : y < that.y); }
	
	static void static_destruction_clean_up( void ) { sprites.clear(); }
	
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
