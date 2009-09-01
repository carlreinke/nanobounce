#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "ball.hpp"
#include "block.hpp"
#include "SDL.h"

class Level
{
public:
	Level( void );
	
	bool load( std::istream & );
	bool load( const std::string & );
	void reset( void );
	
	void draw( SDL_Surface *, int x_offset, int y_offset, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
	const std::string &name;
	
private:
	std::string m_name;
	int width, height;
	std::vector<Block> blocks;
	
	friend class Game;
};

#endif // LEVEL_HPP
