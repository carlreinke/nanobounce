#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "ball.hpp"
#include "block.hpp"
#include "SDL.h"

class Level
{
public:
	Level( void );
	
	void copy( const Level & );
	Level &operator=( const Level &that ) { copy(that); return *this; }
	
	bool load( std::istream & );
	bool load( const std::string & );
	
	bool save( std::ostream & ) const;
	bool save( const std::string & ) const;
	
	void reset( void );
	
	void draw( SDL_Surface *, int x_offset, int y_offset, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
	const std::string &name;
	
private:
	std::string m_name;
	int width, height;
	std::vector<Block> blocks;
	
	static std::map<Block::types, char> block_chars;
	
	friend class Game;
	friend class Editor;
};

#endif // LEVEL_HPP
