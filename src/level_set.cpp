#include "file_system.hpp"
#include "level_set.hpp"
#include "main.hpp"

using namespace std;

LevelSet::LevelSet( void )
: valid(false)
{
	int i = 0;
	do
		directory = level_directory + boost::lexical_cast<string>(++i);
	while (path_exists(directory));
}

LevelSet::LevelSet( const std::string &directory )
: valid(false), directory(directory)
{
	ifstream meta((directory + "/meta").c_str());
	getline(meta, name);
	getline(meta, author);
	
	valid = meta.good();
}

void LevelSet::load_levels( void )
{
	if (invalid())
		return;
	
	ifstream meta((directory + "/meta").c_str());
	
	getline(meta, name);
	getline(meta, author);
	
	string level_path;
	while (getline(meta, level_path))
	{
		boost::trim_right_if(level_path, boost::is_any_of("\r"));
		
		level_path = directory + "/" + level_path;
		
		Level level;
		level.load(level_path);
		
		if (!level.invalid())
			levels.push_back(level);
	}
}

void LevelSet::save_meta( void )
{
	if (!path_exists(directory))
#ifndef TARGET_WIN32
		mkdir(directory.c_str(), 0755);
#else
		mkdir(directory.c_str());
#endif
	
	ofstream meta((directory + "/meta").c_str());
	meta << name << endl;
	meta << author << endl;
	
	BOOST_FOREACH (const Level &level, levels)
		meta << level.path.substr(directory.size() + 1) << endl;
	
	valid = meta.good();
}

void LevelSet::append_level( Level &level )
{
	string::size_type basename_offset = level.path.find_last_of('/');
	if (basename_offset == string::npos || basename_offset == level.path.size() - 1)
		level.path = ".";  // force new path
	else
		level.path = directory + "/" + level.path.substr(basename_offset + 1);
	
	// if path already exists, give level an unused, numeric filename
	if (path_exists(level.path))
	{
		uint i = 0;
		do
			level.path = directory + "/" + boost::lexical_cast<string>(++i);
		while (path_exists(level.path));
		
		level.name += " " + boost::lexical_cast<string>(i);
	}
	
	levels.push_back(level);
}
