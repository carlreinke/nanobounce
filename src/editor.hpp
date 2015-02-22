#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "level.hpp"
#include "loop.hpp"

class Editor : public Loop
{
public:
	Editor( void );
	~Editor( void );
	
	static void static_destruction_clean_up( void ) { block_sprites.clear(); }
	
protected:
	void handle_event( SDL_Event & );
	
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;

public:
	bool load_level( const boost::filesystem::path & );
	bool save_level( const boost::filesystem::path & );

protected:	
	bool load_level_temporary( void );
	bool save_level_temporary( void ) const;
	
	void reset( void );
	
	static int align( int value, int mod ) { return value - value % mod; }
	
	std::vector<LevelBlock>::iterator block_at_position( int x, int y );
	void set_block_at_position( int x, int y, LevelBlock::Type );
	
private:
	int x_offset, y_offset;
	
	int cursor_x, cursor_y;
	uint cursor_block;
	
	boost::filesystem::path level_path;
	Level level;
	
	static std::vector<Sprite> block_sprites;
	static std::bitset<LevelBlock::Type_COUNT> block_type_unusable;
	
	void menu( void );
};

#endif // EDITOR_HPP
