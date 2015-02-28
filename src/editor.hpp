/*  nanobounce/src/editor.hpp
 *  
 *  Copyright 2015 Carl Reinke
 *  
 *  This program is non-commercial, open-source software; you can redistribute
 *  it and/or modify it under the terms of the MAME License as included along
 *  with this program.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  MAME License for more details.
 *  
 *  You should have received a copy of the MAME License along with this
 *  program; if not, see <http://www.intoorbit.org/licenses/MAME.txt>.
 */
#ifndef EDITOR_HPP
#define EDITOR_HPP

#include "level.hpp"
#include "level_pack.hpp"
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
	
	enum MenuResult
	{
		QUIT,
		BACK,
		DONE,
	};
	
	MenuResult menu_save( void );
	MenuResult menu_save( LevelPack & );
	
	MenuResult menu_load( void );
	MenuResult menu_load( LevelPack & );
};

#endif // EDITOR_HPP
