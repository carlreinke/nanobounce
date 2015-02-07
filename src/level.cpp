#include "audio/audio.hpp"
#include "controller/controller.hpp"
#include "level.hpp"
#include "misc.hpp"

boost::bimap<Block::Type, std::string> Level::block_names;

Level::Level( void )
: valid(false),
  width(0), height(0)
{
	load_resources();
}

Level::Level( std::string name, int width, int height )
: valid(true),
  name(name),
  width(width), height(height)
{
	load_resources();
}

bool Level::load( const std::string &data_path )
{
	path = data_path;
	
	std::ifstream data(path.c_str());
	bool success = load(data);
	
	if (success)
		std::cout << "loaded level '" << name << "' from '" << path << "'" << std::endl;
	else
		std::cout << "warning: failed to load level from '" << path << "'" << std::endl;
	
	return success;
}

bool Level::load( std::istream &data )
{
	getline(data, name);
	
	data >> width >> height;
	
	width *= Block::width;
	height *= Block::height;
	
	valid = data.good();
	
	blocks.clear();
	
	int x, y;
	std::string block_name;
	
	while (data >> x >> y >> block_name)
	{
		try
		{
			x *= Block::width;
			y *= Block::height;
			Block::Type type = block_names.right.at(block_name);
			
			blocks.push_back(Block(x, y, type));
		}
		catch (std::out_of_range &e) { assert(false); }
	}
	
	return valid;
}

bool Level::save( const std::string &data_path ) const
{
	path = data_path;
	
	std::ofstream data(path.c_str());
	bool success = save(data);
	
	std::cout << (success ? "saved" : "warning: failed to save") << " level"
	          << " '" << name << "' to '" << data_path << "'" << std::endl;
	
	return success;
}

bool Level::save( std::ostream &data ) const
{
	if (invalid())
		return false;
	
	data << name << std::endl;
	
	data << (width / Block::width) << " "
	     << (height / Block::height) << std::endl;
	
	for (const Block &block : blocks)
	{
		data << (block.x / block.width) << " "
		     << (block.y / block.height) << " "
		     << block_names.left.at(block.type) << std::endl;
	}
	
	return data.good();
}

void Level::validate( void )
{
	// remove blocks outside boundaries
	blocks.erase(std::remove_if(blocks.begin(), blocks.end(),
		[this]( const Block &block ) { return block.x < 0 ||
		                                      block.y < 0 ||
		                                      block.x >= width ||
		                                      block.y >= height; }));
	
	stable_sort(blocks.begin(), blocks.end());
}

void Level::reset( void )
{
	for (Block &block : blocks)
		block.reset();
}

void Level::draw( SDL_Surface *surface, int x_offset, int y_offset, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	for (const Block &block : blocks)
		block.draw(surface, x_offset, y_offset, alpha);
}

void Level::load_resources( void )
{
	if (block_names.empty())
	{
		boost::array<std::pair<Block::Type, std::string>, Block::Type_COUNT> temp_block_names =
		{{
			std::make_pair(Block::BALL,    "ball"),
			std::make_pair(Block::EXIT,    "exit"),
			std::make_pair(Block::NORMAL,  "block"),
			std::make_pair(Block::NUKE,    "nuke"),
			std::make_pair(Block::RECYCLE, "recycle"),
			
			std::make_pair(Block::TOGGLE_0_0,    "toggle_0"),
			std::make_pair(Block::TOGGLE_0_STAR, "toggle_0_star"),
			std::make_pair(Block::TOGGLE_1_1,    "toggle_1"),
			std::make_pair(Block::TOGGLE_1_STAR, "toggle_1_star"),
			
			std::make_pair(Block::BOOST_UP,    "boost_up"),
			std::make_pair(Block::BOOST_LEFT,  "boost_left"),
			std::make_pair(Block::BOOST_RIGHT, "boost_right"),
			
			std::make_pair(Block::PUSH_UP,    "push_up"),
			std::make_pair(Block::PUSH_LEFT,  "push_left"),
			std::make_pair(Block::PUSH_RIGHT, "push_right"),
		}};
		
		typedef std::pair<Block::Type, std::string> BlockPair;
		for (const BlockPair &i : temp_block_names)
			block_names.insert(make_bipair(i.first, i.second));
	}
}
