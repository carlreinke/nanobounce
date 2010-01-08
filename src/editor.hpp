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
	
	int align( int value, int mod ) { return value - value % mod; }
	
	std::vector<Block>::iterator block_at_position( int x, int y );
	void set_block_at_position( int x, int y, Block::Type );
	
private:
	int x_offset, y_offset;
	
	int cursor_x, cursor_y;
	uint cursor_block;
	
	Level level;
	
	static std::vector<Sprite> block_sprites;
	static Sprite cursor;
	
	void menu( void );
};

#endif // EDITOR_HPP
