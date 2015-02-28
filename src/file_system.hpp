/*  nanobounce/src/filesystem.hpp
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
#ifndef FILE_SYSTEM_HPP
#define FILE_SYSTEM_HPP

extern boost::filesystem::path user_data_directory;

extern std::vector<std::string> list_directories( const boost::filesystem::path &path );

extern std::vector<std::string> list_files( const boost::filesystem::path &path );

extern std::string sanitize_filename( const std::string &filename );

#endif // FILE_SYSTEM_HPP
