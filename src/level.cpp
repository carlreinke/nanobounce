#include "audio/audio.hpp"
#include "controller/controller.hpp"
#include "level.hpp"
#include "misc.hpp"

using namespace std;

boost::bimap<Block::Type, string> Level::block_names;

Level::Level( void )
: valid(false),
  path("invalid"), name("invalid"),
  width(0), height(0)
{
	if (block_names.empty())
	{
		boost::array<pair<Block::Type, string>, Block::_max> temp_block_names =
		{{
			make_pair(Block::none,    ""),
			make_pair(Block::ball,    "ball"),
			make_pair(Block::exit,    "exit"),
			make_pair(Block::normal,  "block"),
			make_pair(Block::nuke,    "nuke"),
			make_pair(Block::recycle, "recycle"),
			
			make_pair(Block::toggle_0_0,    "toggle_0"),
			make_pair(Block::toggle_0_star, "toggle_0_star"),
			make_pair(Block::toggle_1_1,    "toggle_1"),
			make_pair(Block::toggle_1_star, "toggle_1_star"),
			
			make_pair(Block::boost_up,    "boost_up"),
			make_pair(Block::boost_left,  "boost_left"),
			make_pair(Block::boost_right, "boost_right"),
			
			make_pair(Block::push_up,    "push_up"),
			make_pair(Block::push_left,  "push_left"),
			make_pair(Block::push_right, "push_right"),
		}};
		
		typedef pair<Block::Type, string> BlockPair;
		BOOST_FOREACH (const BlockPair &i, temp_block_names)
			block_names.insert(make_bipair(i.first, i.second));
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
	
	valid = data.good();
	
	blocks.clear();
	
	int x, y;
	string block_name;
	
	while (data >> x >> y >> block_name)
	{
		try
		{
			x *= Block::width;
			y *= Block::height;
			Block::Type type = block_names.right.at(block_name);
			
			blocks.push_back(Block(x, y, type));
		}
		catch (exception e)
		{
			// ignore it
		}
	}
	
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
			     << block_names.left.at(block.type) << endl;
		}
	}
	
	return data.good();
}

void Level::validate( void )
{
	// remove blocks outside boundaries
	for (vector<Block>::iterator block = blocks.begin(); block != blocks.end(); )
	{
		if (block->x >= width || block->y >= height)
			block = blocks.erase(block);
		else
			++block;
	}
	
	stable_sort(blocks.begin(), blocks.end());
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
