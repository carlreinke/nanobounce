#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "level.hpp"
#include "loop.hpp"

class Editor : public Loop
{
public:
	Editor( void );
	
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
	bool load( const std::string &level_data_path );
	bool save( const std::string &level_data_path ) const;
	
	void reset( void );
	
private:
	int x_offset, y_offset;
	
	int cursor_x, cursor_y;
	
	Level level;
	
	static std::map<Block::types, Sprite> block_sprites;
};

#endif // EDITOR_HPP
