#ifndef LEVEL_HPP
#define LEVEL_HPP

#include "ball.hpp"
#include "block.hpp"

#include <SDL.h>

class Level
{
public:
	Level( void );
	Level( std::string name, int width, int height );
	bool invalid( void ) const { return !valid; }
	
	bool load( void ) { return load(path); }
	bool load( const std::string &path );
	bool load( std::istream & );
	
	bool save( void ) const { return save(path); }
	bool save( const std::string &path ) const;
	bool save( std::ostream & ) const;
	
	void reset( void );
	
	void draw( SDL_Surface *, int x_offset, int y_offset, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
	std::string get_path( void ) const { return path; }
	std::string get_name( void ) const { return name; }
	
	std::string get_score_path( void ) const { return (path.size() ? path + ".score" : ""); }
	
private:
	bool valid;
	mutable std::string path;
	
	void validate( void );
	
	std::string name;
	int width, height;
	std::vector<Block> blocks;
	
	static void load_resources( void );
	static boost::bimap<Block::Type, std::string> block_names;
	
	friend class Editor;
	friend class Game;
	friend class LevelPack;
};

#endif // LEVEL_HPP
