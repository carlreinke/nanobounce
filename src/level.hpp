#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "ball.hpp"
#include "block.hpp"
#include "hash/sha256.h"

#include <SDL.h>

class Level
{
public:
	Level( void );
	explicit Level( const boost::filesystem::path & );
	
	bool invalid( void ) const { return !valid; }
	
	bool load( const boost::filesystem::path & );
	bool load( std::istream & );
	
	bool save( const boost::filesystem::path & ) const;
	bool save( std::ostream & ) const;
	
	void normalize( void );
	
	void draw( SDL_Surface *, int x_offset, int y_offset, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
	const std::string & get_name( void ) const { return name; }
	
	int get_width( void ) const { return width; }
	int get_height( void ) const { return height; }
	
	const std::vector<LevelBlock> &get_blocks( void ) const { return blocks; }
	
	sha256 calculate_hash( void ) const;
	
private:
	bool valid;
	
	std::string name;
	
	int width, height;
	
	std::vector<LevelBlock> blocks;
	
	static void load_resources( void );
	static boost::bimap<LevelBlock::Type, std::string> block_names;
	
	friend class Editor;
};

#endif // LEVEL_HPP
