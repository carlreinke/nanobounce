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
		
		block_chars[Block::push_up] = 'u';
		block_chars[Block::push_left] = 'l';
		block_chars[Block::push_right] = 'r';
	}
}

void Level::copy( const Level &that )
{
	m_name = that.m_name;
	width = that.width;
	height = that.height;
	blocks = that.blocks;
}

bool Level::load( istream &data )
{
	getline(data, m_name);
	
	data >> width >> height;
	
	width *= Block::width;
	height *= Block::height;
	
	blocks.clear();
	
	const map<char, Block::types> block_chars_temp = flip(block_chars);
	
	int x, y;
	char type_char;
	
	while (data >> x >> y >> type_char)
	{
		x *= Block::width;
		y *= Block::height;
		Block::types type = block_chars_temp.find(type_char)->second;
		
		blocks.push_back(Block(x, y, type));
	}
	
	if (data.eof() && !blocks.empty())
		cout << "loaded level '" << name << "'" << endl;
	else
		cout << "warning: level failed to load" << endl;
	
	return (data.eof() && !blocks.empty());
}

bool Level::load( const string &data_path )
{
	ifstream data(data_path.c_str());
	return load(data);
}

bool Level::save( ostream &data ) const
{
	data << m_name << endl;
	
	data << (width / Block::width) << " "
	     << (height / Block::height) << endl;
	
	for (vector<Block>::const_iterator i = blocks.begin(); i != blocks.end(); ++i)
	{
		if (i->type != Block::none)
		{
			data << (i->x / Block::width) << " "
			     << (i->y / Block::height) << " "
			     << block_chars[i->type] << endl;
		}
	}
	
	if (data.good())
		cout << "saved level '" << name << "'" << endl;
	else
		cout << "warning: level '" << name << "' failed to save" << endl;
	
	return data.good();
}

bool Level::save( const string &data_path ) const
{
	ofstream data(data_path.c_str());
	return save(data);
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
