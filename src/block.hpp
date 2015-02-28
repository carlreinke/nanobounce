/*  nanobounce/src/block.hpp
 *  
 *  Copyright 2015 Carl Reinke
 *  
 *  This program is non-commercial, open-source software; you can redistribute
 *  it and/or modify it under the terms of the MAME License as included along
 *  with this program.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  MAME License for more details.
 *  
 *  You should have received a copy of the MAME License along with this
 *  program; if not, see <http://www.intoorbit.org/licenses/MAME.txt>.
 */
#ifndef BLOCK_HPP
#define BLOCK_HPP

#include "video/sprite.hpp"

#include <SDL.h>

class LevelBlock
{
public:
	enum Type
	{
		NONE = 0,
		BALL = 1,
		EXIT = 2,
		
		NORMAL  = 3,
		NUKE    = 4,
		RECYCLE = 5,
		
		TOGGLE_0_0    = 6,
		TOGGLE_0_1    = 7,
		TOGGLE_0_STAR = 8,
		
		TOGGLE_1_0    = 9,
		TOGGLE_1_1    = 10,
		TOGGLE_1_STAR = 11,
		
		BOOST_UP    = 12,
		BOOST_LEFT  = 13,
		BOOST_RIGHT = 14,
		
		PUSH_UP    = 15,
		PUSH_LEFT  = 16,
		PUSH_RIGHT = 17,
		
		Type_COUNT
	};
	
	LevelBlock( int x, int y, Type type );
	
	bool operator==( const LevelBlock &that ) const { return y == that.y &&
	                                                         x == that.x &&
	                                                         type == that.type; }
	bool operator<( const LevelBlock &that ) const { return y != that.y ? y < that.y :
	                                                        x != that.x ? x < that.x :
	                                                                      type < that.type; }
	
	static void static_destruction_clean_up( void ) { sprites.clear(); }
	
	int get_x( void ) const { return x; }
	int get_y( void ) const { return y; }

	Type get_type( void ) const { return type; }
	
	void draw( SDL_Surface *, int x_offset, int y_offset, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
	static const int width, height;
	
protected:
	int x, y;
	
	Type type;
	
	static std::vector<Sprite> sprites;
	
	friend class Editor;
	friend class GameBlock;
};

class GameBlock : public LevelBlock
{
public:
	GameBlock( const LevelBlock &block ) : LevelBlock(block.x, block.y, block.type) { ingame_reset(); }

	void ingame_reset( void );
		
	Type ingame_type;  // intrusive member used in-game
	
	enum Property
	{
		HIDDEN,
		IGNORED,
		COLLIDABLE,
		TRIGGERABLE
	};
	
	Property ingame_property;  // intrusive member used in-game
	
	void draw( SDL_Surface *, int x_offset, int y_offset, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;

	friend class Game;
};

#endif // BLOCK_HPP
