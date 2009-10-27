#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "level.hpp"

class Editor
{
public:
	Editor( void );
	
	bool load( const std::string &level_data_path );
	bool save( const std::string &level_data_path ) const;
	
	void reset( void );
	
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
private:
	int x_offset, y_offset;
	
	int cursor_x, cursor_y;
	
	Level level;
	
	static std::map<Block::types, Sprite> block_sprites;
};

void editor_loop( SDL_Surface *surface, Editor &editor );

#endif // EDITOR_HPP
