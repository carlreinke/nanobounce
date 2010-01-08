#include "audio.hpp"
#include "controller.hpp"
#include "level.hpp"
#include "misc.hpp"
#include "SDL.h"

using namespace std;

map<Block::Type, char> Level::block_chars;

Level::Level( void )
: valid(false)
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

bool Level::load( const string &data_path )
{
	path = data_path;
	
	ifstream data(path.c_str());
	
	bool success = load(data);
	
	if (success)
		cout << "loaded level '" << name << "' from '" << path << "'" << endl;
	else
		cout << "warning: failed to load level from '" << path << "'" << endl;
	
	return success;
}

bool Level::load( istream &data )
{
	getline(data, name);
	
	data >> width >> height;
	
	width *= Block::width;
	height *= Block::height;
	
	blocks.clear();
	
	const map<char, Block::Type> block_chars_temp = flip(block_chars);
	
	int x, y;
	char type_char;
	
	while (data >> x >> y >> type_char)
	{
		x *= Block::width;
		y *= Block::height;
		Block::Type type = block_chars_temp.find(type_char)->second;
		
		blocks.push_back(Block(x, y, type));
	}
	
	valid = data.eof() && !blocks.empty();
	
	return valid;
}

bool Level::save( void ) const
{
	return save(path);
}

bool Level::save( const string &data_path ) const
{
	ofstream data(data_path.c_str());
	
	bool success = save(data);
	
	cout << (success ? "saved" : "warning: failed to save") << " level"
	     << " '" << name << "' to '" << data_path << "'" << endl;
	
	return success;
}

bool Level::save( ostream &data ) const
{
	if (invalid())
		return false;
	
	data << name << endl;
	
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
	
	return data.good();
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
