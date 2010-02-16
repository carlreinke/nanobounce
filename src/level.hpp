#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "ball.hpp"
#include "block.hpp"
#include "SDL.h"

class Level
{
public:
	Level( void );
	bool invalid( void ) const { return !valid; }
	
	bool load( const std::string &path );
	bool load( std::istream & );
	
	bool save( void ) const;
	bool save( const std::string &path ) const;
	bool save( std::ostream & ) const;
	
	void reset( void );
	
	void draw( SDL_Surface *, int x_offset, int y_offset, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
private:
	void validate( void );
	
	bool valid;
	std::string path;
	
	std::string name;
	int width, height;
	std::vector<Block> blocks;
	
	static boost::bimap<Block::Type, std::string> block_names;
	
	friend class Editor;
	friend class Game;
	friend class GameMenu;
	friend class LevelMenu;
	friend class LevelSet;
	friend class LevelIntroLoop;
};

#endif // LEVEL_HPP
