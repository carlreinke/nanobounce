#include "audio/audio.hpp"
#include "controller/controller.hpp"
#include "level.hpp"
#include "misc.hpp"

using namespace std;

boost::bimap<Block::Type, char> Level::block_chars;

Level::Level( void )
: valid(false),
  path("invalid"), name("invalid"),
  width(0), height(0)
{
	if (block_chars.empty())
	{
		block_chars.insert(make_bipair(Block::none, ' '));
		block_chars.insert(make_bipair(Block::ball, '.'));
		block_chars.insert(make_bipair(Block::exit, 'x'));
		block_chars.insert(make_bipair(Block::normal, '='));
		block_chars.insert(make_bipair(Block::nuke, '*'));
		block_chars.insert(make_bipair(Block::recycle, '-'));
		
		block_chars.insert(make_bipair(Block::locked, '~'));
		block_chars.insert(make_bipair(Block::locked_star, '!'));
		
		block_chars.insert(make_bipair(Block::boost_up, '^'));
		block_chars.insert(make_bipair(Block::boost_left, '<'));
		block_chars.insert(make_bipair(Block::boost_right, '>'));
		
		block_chars.insert(make_bipair(Block::push_up, 'u'));
		block_chars.insert(make_bipair(Block::push_left, 'l'));
		block_chars.insert(make_bipair(Block::push_right, 'r'));
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
	
	int x, y;
	char type_char;
	
	while (data >> x >> y >> type_char)
	{
		try
		{
			x *= Block::width;
			y *= Block::height;
			Block::Type type = block_chars.right.at(type_char);
			
			blocks.push_back(Block(x, y, type));
		}
		catch (exception e)
		{
			// ignore it
		}
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
	
	BOOST_FOREACH (const Block &block, blocks)
	{
		if (block.type != Block::none)
		{
			data << (block.x / block.width) << " "
			     << (block.y / block.height) << " "
			     << block_chars.left.at(block.type) << endl;
		}
	}
	
	return data.good();
}

void Level::reset( void )
{
	BOOST_FOREACH (Block &block, blocks)
		block.reset();
}

void Level::draw( SDL_Surface *surface, int x_offset, int y_offset, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	BOOST_FOREACH (const Block &block, blocks)
		block.draw(surface, x_offset, y_offset, alpha);
}
