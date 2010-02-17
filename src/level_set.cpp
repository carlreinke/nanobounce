#include "audio/audio.hpp"
#include "file_system.hpp"
#include "game.hpp"
#include "game_loops.hpp"
#include "level_set.hpp"
#include "main.hpp"
#include "video/font.hpp"
#include "volume.hpp"

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
	
	level.valid = true;
	
	levels.push_back(level);
}

void LevelSet::play( SDL_Surface *surface )
{
	if (invalid())
		return;
	
	if (levels.empty())
		load_levels();
	
	Game game;
	
	vector<Level>::iterator level = levels.begin();
	
	string highscore_path;
	Highscore highscore;
	
	while (game.state != Game::quit && level != levels.end() && !global_quit)
	{
		if (game.state == Game::lost)
		{
			// retry level
			game.reset();
		}
		else
		{
			game = Game(*level);
			
			highscore.load(level->path + ".score");
			
			LevelIntroLoop level_intro(*level, highscore);
			level_intro.loop(surface);
		}
		
		game.loop(surface);
		
		if (game.state == Game::won)
		{
			if (game.highscore.ms() < highscore.ms() || highscore.invalid())
			{
				LevelCongratsLoop congrats(*level, game.highscore);
				congrats.loop(surface);
				
				// TODO: highscore screen, ask for name?
				
				game.highscore.save();
			}
		}
		
		if (game.state == Game::won || game.state == Game::cheat_won)
			++level;
	}
	
	if (level == levels.end())
	{
		LevelSetCongratsLoop congrats(*this);
		congrats.loop(surface);
	}
}
