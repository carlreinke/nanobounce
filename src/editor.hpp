#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "level.hpp"
#include "loop.hpp"

class Editor : public Loop
{
public:
	Editor( void );
	
	void handle_event( SDL_Event & );
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
	bool load( const std::string &level_data_path );
	bool save( const std::string &level_data_path ) const;
	
	void reset( void );
	
	void set_block_at_position( int x, int y, Block::types );
	
private:
	int x_offset, y_offset;
	
	int cursor_x, cursor_y;
	Block::types cursor_block;
	
	Level level;
	
	static std::map<Block::types, Sprite> block_sprites;
	static Sprite cursor;
};

#endif // EDITOR_HPP
