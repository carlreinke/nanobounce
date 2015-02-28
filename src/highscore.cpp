/*  nanobounce/src/highscore.cpp
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
#include "highscore.hpp"
#include "loop.hpp"
#include "main.hpp"

Highscore::Highscore( void )
: valid(false),
  level_hash({ 0, 0, 0, 0, 0, 0, 0, 0 }),
  ticks_per_second(update_per_sec),
  ticks(0)
{
	// nothing more to do
}

Highscore::Highscore( const sha256 &level_hash )
: valid(true),
  level_hash(level_hash),
  ticks_per_second(update_per_sec),
  ticks(0)
{
	// nothing more to do
}

Highscore::Highscore( const boost::filesystem::path &path )
{
	load(path);
}

bool Highscore::load( const boost::filesystem::path &path )
{
	std::ifstream stream(path.string());
	
	bool success = load(stream);
	
	if (success)
		std::cout << "loaded score '" << player_name << "' (" << get_time_ms() << " ms) from '" << path.string() << "'" << std::endl;
	else
		std::cout << "warning: failed to load score from '" << path.string() << "'" << std::endl;
	
	return success;
}

bool Highscore::load( std::istream &stream )
{
	std::string hash;  // TODO
	
	std::getline(stream, hash);
	boost::trim_right_if(hash, boost::is_any_of("\r"));
	
	std::getline(stream, player_name);
	boost::trim_right_if(player_name, boost::is_any_of("\r"));
	
	stream >> ticks_per_second >> ticks;
	
	valid = stream.good();
	
	ball_control_history.clear();
	for (; ; )
	{
		int tick;
		int x_direction;
		
		stream >> tick >> x_direction;
		
		if (!stream.good())
			break;
		
		if (tick < ticks)
			this->ball_control_history.emplace_back(tick, x_direction);
	}
	
	return valid &= stream.eof();
}

bool Highscore::save( const boost::filesystem::path &path ) const
{
	if (!valid)
		return false;
	
	boost::filesystem::create_directories(path.parent_path());
	
	std::ofstream stream(path.string());
	
	bool success = save(stream);
	
	std::cout << (success ? "saved" : "warning: failed to save") << " score"
	          << " '" << player_name << "' " << get_time_ms() << " ms to '" << path << "'" << std::endl;
	
	return success;
}

bool Highscore::save( std::ostream &stream ) const
{
	stream << std::hex;
	
	for (int i = 0; i < 8; ++i)
		stream << std::setw(8) << std::setfill('0') << level_hash.h[i];
	stream << std::endl;
	
	stream << std::dec;
	
	stream << player_name << std::endl;
	
	stream << ticks_per_second << " "
	       << ticks << std::endl;
	
	for (const auto ball_control_entry : ball_control_history)
		stream << ball_control_entry.tick << " " << ball_control_entry.x_direction << " ";
	stream << ticks << " " << 0 << std::endl;
	
	return stream.good();
}

void Highscore::push_back_tick( int x_direction )
{
	// if direction different than last tick, store it
	if (this->ball_control_history.size() == 0 || this->ball_control_history.back().x_direction != x_direction)
		this->ball_control_history.emplace_back(ticks, x_direction);
	
	++ticks;
}

std::string Highscore::format_time( int ms_temp )
{
	// split milliseconds into min, sec, ms
	int ms = ms_temp % 1000;
	ms_temp -= ms;
	ms_temp /= 1000;
	int sec = ms_temp % 60;
	ms_temp -= sec;
	ms_temp /= 60;
	int min = ms_temp;
	
	std::ostringstream out;
	out << min << ":" << std::setfill('0') << std::setw(2) << sec << "." << std::setfill('0') << std::setw(3) << ms;
	return out.str();
}

Replay::Replay( const Highscore &score )
: highscore(score),
  ticks(0),
  ball_control_index(0)
{
	// good to go
}

void Replay::tick_update( void )
{
	is_down[QUIT] = ticks > (highscore.ticks + highscore.ticks_per_second);
	
	if (ball_control_index < highscore.ball_control_history.size() &&
	    highscore.ball_control_history[ball_control_index].tick == ticks)
	{
		switch (highscore.ball_control_history[ball_control_index].x_direction)
		{
		case -1:
			is_down[LEFT] = true;
			is_down[RIGHT] = false;
			break;
		case 0:
			is_down[LEFT] = false;
			is_down[RIGHT] = false;
			break;
		case 1:
			is_down[LEFT] = false;
			is_down[RIGHT] = true;
			break;
		default:
			break;
		}
		
		++ball_control_index;
	}
	
	++ticks;
}

void Replay::update_down( void )
{
	// nothing to do
}
