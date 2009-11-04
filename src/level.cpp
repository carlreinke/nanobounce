#include "audio.hpp"
#include "controller.hpp"
#include "level.hpp"
#include "misc.hpp"
#include "SDL.h"

using namespace std;

map<Block::types, char> Level::block_chars;

Level::Level( void )
: name(m_name)
{
	if (block_chars.empty())
	{
		block_chars[Block::none] = ' ';
		block_chars[Block::ball] = '.';
		block_chars[Block::exit] = 'x';
		block_chars[Block::normal] = '=';
		block_chars[Block::nuke] = '*';
		block_chars[Block::recycle] = '-';
		block_chars[Block::boost_up] = '^';
		block_chars[Block::boost_left] = '<';
		block_chars[Block::boost_right] = '>';
	}
}

bool Level::load( istream &data )
{
	getline(data, m_name);
	
	{
		istringstream line;
		getline(data, line);
		
		line >> width >> height;
		
		// TODO levels should be allowed an arbitrary size
		width *= Block::width;
		height *= Block::height;
	}
	
	blocks.clear();
	
	for (int y = 0; y < height && data.good(); y += Block::height)
	{
		istringstream line;
		getline(data, line);
		
		map<char, Block::types> block_chars_temp = flip(block_chars);
		
		for (int x = 0; x < width; x += Block::width)
		{
			char type_char;
			line.get(type_char);
			line.ignore(1);
			
			Block::types type = block_chars_temp.find(type_char)->second;
			
			// TODO level format should change to include only existing blocks
			if (type != Block::none)
				blocks.push_back(Block(x, y, type));
		}
	}
	
	if (data.good())
		cout << "loaded level '" << name << "'" << endl;
	else
		cout << "warning: level failed to load" << endl;
	
	return data.good();
}

bool Level::load( const string &data_path )
{
	ifstream data(data_path.c_str());
	return load(data);
}

void Level::reset( void )
{
	for (vector<Block>::iterator block = blocks.begin(); block != blocks.end(); ++block)
		block->reset();
}

void Level::draw( SDL_Surface *surface, int x_offset, int y_offset, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	for (vector<Block>::const_iterator block = blocks.begin(); block != blocks.end(); ++block)
		block->draw(surface, x_offset, y_offset, alpha);
}
