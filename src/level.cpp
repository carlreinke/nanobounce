#include "audio.hpp"
#include "controller.hpp"
#include "level.hpp"
#include "misc.hpp"
#include "SDL.h"

using namespace std;

Level::Level( void )
: name(m_name)
{
	// good to go
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
		
		for (int x = 0; x < width; x += Block::width)
		{
			char type;
			line.get(type);
			line.ignore(1);
			
			// TODO level format should change to include only existing blocks
			if ((Block::types)type != Block::none)
				blocks.push_back(Block(x, y, (Block::types)type));
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
