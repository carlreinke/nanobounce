/*  nanobounce/src/main.hpp
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
#ifndef MAIN_HPP
#define MAIN_HPP

extern bool global_quit;

extern const boost::filesystem::path level_directory;
extern const boost::filesystem::path music_directory;
extern const boost::filesystem::path sample_directory;
extern const boost::filesystem::path sprite_directory;

extern std::string player_name;

#endif // MAIN_HPP
