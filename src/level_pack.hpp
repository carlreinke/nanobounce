/*  nanobounce/src/level_pack.hpp
 *  
 *  Copyright 2015 Carl Reinke
 *  
 *  This program is non-commercial, open-source software; you can redistribute
 *  it and/or modify it under the terms of the MAME License as included along
 *  with this program.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  MAME License for more details.
 *  
 *  You should have received a copy of the MAME License along with this
 *  program; if not, see <http://www.intoorbit.org/licenses/MAME.txt>.
 */
#ifndef LEVEL_PACK_HPP
#define LEVEL_PACK_HPP

#include "level.hpp"

class LevelPack
{
public:
	LevelPack( void );
	LevelPack( const std::string &name, const std::string &author );
	
	LevelPack( const boost::filesystem::path &level_directory, const boost::filesystem::path &score_directory );
	
	bool invalid( void ) const { return !valid; }
	
	bool load_meta( void );
	bool save_meta( void );
	
	const std::string & get_name( void ) const { return name; }
	const std::string & get_author( void ) const { return author; }
	
	const std::vector<boost::filesystem::path> & get_level_filenames( void ) const { return level_filenames; }
	
	uint get_levels_count( void ) const { return level_filenames.size(); }
	boost::filesystem::path get_level_path( uint i ) const;
	boost::filesystem::path get_score_path( uint i ) const;
	
	std::vector<Level> load_levels( void ) const;
	
	boost::filesystem::path generate_level_filename( Level &level ) const;

private:
	bool valid;
	
	boost::filesystem::path level_directory;
	boost::filesystem::path score_directory;
	
	std::string name;
	std::string author;
		
	std::vector<boost::filesystem::path> level_filenames;
	
	friend class Editor;
};

#endif // LEVEL_PACK_HPP
