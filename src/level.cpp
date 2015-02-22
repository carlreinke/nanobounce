#include "audio/audio.hpp"
#include "controller/controller.hpp"
#include "hash/sha256.h"
#include "level.hpp"
#include "misc.hpp"

boost::bimap<LevelBlock::Type, std::string> Level::block_names;

Level::Level( void )
: valid(true),
  width(0), height(0)
{
	load_resources();
}

Level::Level( const boost::filesystem::path &path )
{
	load_resources();
	
	load(path);
}

bool Level::load( const boost::filesystem::path &path )
{
	std::ifstream stream(path.c_str());
	bool success = load(stream);
	
	if (success)
		std::cout << "loaded level '" << name << "' from '" << path.string() << "'" << std::endl;
	else
		std::cout << "warning: failed to load level from '" << path.string() << "'" << std::endl;
	
	return success;
}

bool Level::load( std::istream &stream )
{
	std::getline(stream, name);
	boost::trim_right_if(name, boost::is_any_of("\r"));
	
	stream >> width >> height;
	
	valid = stream.good();
	
	width *= LevelBlock::width;
	height *= LevelBlock::height;
	
	blocks.clear();
	for (; ; )
	{
		int x, y;
		std::string block_name;
		
		stream >> x >> y >> block_name;
		
		if (!stream.good())
			break;
		
		try
		{
			x *= LevelBlock::width;
			y *= LevelBlock::height;
			LevelBlock::Type type = block_names.right.at(block_name);
			
			blocks.push_back(LevelBlock(x, y, type));
		}
		catch (std::out_of_range &e) { assert(false); }
	}
	
	return valid &= stream.eof();
}

bool Level::save( const boost::filesystem::path &path ) const
{
	boost::filesystem::create_directories(path.parent_path());
		
	std::ofstream data(path.c_str());
	bool success = save(data);
	
	std::cout << (success ? "saved" : "warning: failed to save") << " level"
	          << " '" << name << "' to '" << path << "'" << std::endl;
	
	return success;
}

bool Level::save( std::ostream &stream ) const
{
	if (!valid)
		return false;
	
	stream << name << std::endl;
	
	stream << (width / LevelBlock::width) << " "
	       << (height / LevelBlock::height) << std::endl;
	
	for (const LevelBlock &block : blocks)
	{
		stream << (block.get_x() / block.width) << " "
		       << (block.get_y() / block.height) << " "
		       << block_names.left.at(block.get_type()) << std::endl;
	}
	
	return stream.good();
}

void Level::normalize( void )
{
	// remove blocks outside boundaries
	blocks.erase(std::remove_if(blocks.begin(), blocks.end(),
		[this]( const LevelBlock &block )
		{
			return block.get_x() < 0 ||
			       block.get_y() < 0 ||
			       block.get_x() >= width ||
			       block.get_y() >= height; }), blocks.end());
	
	std::sort(blocks.begin(), blocks.end());
	
	blocks.erase(std::unique(blocks.begin(), blocks.end()), blocks.end());
}

void Level::draw( SDL_Surface *surface, int x_offset, int y_offset, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	for (const LevelBlock &block : blocks)
		block.draw(surface, x_offset, y_offset, alpha);
}

sha256 Level::calculate_hash( void ) const
{
	sha256_stream sha256;
	
	sha256_stream_init(&sha256);
	
	for (const auto &block : blocks)
	{
		const uint32_t data[] = {
			SDL_SwapBE32(static_cast<uint32_t>(block.get_x())),
			SDL_SwapBE32(static_cast<uint32_t>(block.get_y())),
			SDL_SwapBE32(static_cast<uint32_t>(block.get_type())),
		};
		
		sha256_stream_feed(&sha256, reinterpret_cast<const uint8_t *>(&data), sizeof data);
	}
	
	sha256_stream_finish(&sha256);
	
	return sha256.digest;
}

void Level::load_resources( void )
{
	if (block_names.empty())
	{
		boost::array<std::pair<LevelBlock::Type, std::string>, LevelBlock::Type_COUNT> temp_block_names =
		{{
			std::make_pair(LevelBlock::BALL,    "ball"),
			std::make_pair(LevelBlock::EXIT,    "exit"),
			std::make_pair(LevelBlock::NORMAL,  "block"),
			std::make_pair(LevelBlock::NUKE,    "nuke"),
			std::make_pair(LevelBlock::RECYCLE, "recycle"),
			
			std::make_pair(LevelBlock::TOGGLE_0_0,    "toggle_0"),
			std::make_pair(LevelBlock::TOGGLE_0_STAR, "toggle_0_star"),
			std::make_pair(LevelBlock::TOGGLE_1_1,    "toggle_1"),
			std::make_pair(LevelBlock::TOGGLE_1_STAR, "toggle_1_star"),
			
			std::make_pair(LevelBlock::BOOST_UP,    "boost_up"),
			std::make_pair(LevelBlock::BOOST_LEFT,  "boost_left"),
			std::make_pair(LevelBlock::BOOST_RIGHT, "boost_right"),
			
			std::make_pair(LevelBlock::PUSH_UP,    "push_up"),
			std::make_pair(LevelBlock::PUSH_LEFT,  "push_left"),
			std::make_pair(LevelBlock::PUSH_RIGHT, "push_right"),
		}};
		
		typedef std::pair<LevelBlock::Type, std::string> BlockPair;
		for (const BlockPair &i : temp_block_names)
			block_names.insert(make_bipair(i.first, i.second));
	}
}
