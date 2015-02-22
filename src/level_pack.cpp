#include "file_system.hpp"
#include "level_pack.hpp"
#include "main.hpp"

LevelPack::LevelPack( void )
: valid(true)
{
	// nothing to do
}

LevelPack::LevelPack( const std::string &name, const std::string &author )
: valid(true),
  name(name), author(author)
{
	// nothing to do
}

LevelPack::LevelPack( const boost::filesystem::path &level_directory, const boost::filesystem::path &score_directory )
: level_directory(level_directory),
  score_directory(score_directory)
{
	load_meta();
}

bool LevelPack::load_meta( void )
{
	std::ifstream stream((level_directory / "meta").c_str());
	
	std::getline(stream, name);
	boost::trim_right_if(name, boost::is_any_of("\r"));
	std::getline(stream, author);
	boost::trim_right_if(author, boost::is_any_of("\r"));
	
	valid = stream.good();
	
	std::string level_filename;
	while (std::getline(stream, level_filename))
	{
		boost::trim_right_if(level_filename, boost::is_any_of("\r"));
		
		if (!level_filename.empty())
			level_filenames.push_back(level_filename);
	}
	
	return valid &= stream.eof();
}

bool LevelPack::save_meta( void )
{
	if (!valid)
		return false;
	
	if (level_directory.empty())
	{
		const auto user_levels_directory = user_data_directory / "levels";
		
		const std::string sanitized_name = sanitize_filename(name);
			
		level_directory = user_levels_directory / sanitized_name;
		
		unsigned int i = 0;
		while (boost::filesystem::exists(level_directory))
			level_directory = user_levels_directory / (sanitized_name + boost::lexical_cast<std::string>(++i));
	}
	
	if (!boost::filesystem::exists(level_directory))
		boost::filesystem::create_directories(level_directory);
	
	std::ofstream stream((level_directory / "meta").c_str());
	
	stream << name << std::endl;
	stream << author << std::endl;
	
	for (const auto &level_filename : level_filenames)
		stream << level_filename.string() << std::endl;
	
	return stream.good();
}

boost::filesystem::path LevelPack::get_level_path( uint i ) const
{
	return level_directory / level_filenames[i];
}

boost::filesystem::path LevelPack::get_score_path( uint i ) const
{
	auto score_path = score_directory / level_filenames[i];
	score_path.replace_extension(".score");
	return score_path;
}

std::vector<Level> LevelPack::load_levels( void ) const
{
	std::vector<Level> levels;
	
	for (uint i = 0; i < level_filenames.size(); ++i)
	{
		Level level;
		level.load(get_level_path(i));
		levels.push_back(level);
	}
	
	return levels;
}

boost::filesystem::path LevelPack::generate_level_filename( Level &level ) const
{
	const std::string sanitized_level_name = sanitize_filename(level.get_name());
	
	boost::filesystem::path level_filename = sanitized_level_name;

	unsigned int i = 0;
	while (boost::filesystem::exists(level_directory / level_filename))
		level_filename = sanitized_level_name + boost::lexical_cast<std::string>(++i);
	
	return level_filename;
}
