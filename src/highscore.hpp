#ifndef HIGHSCORE_HPP
#define HIGHSCORE_HPP

#include "controller/controller.hpp"
#include "hash/sha256.h"

class Highscore
{
public:
	Highscore( void );
	explicit Highscore( const sha256 &level_hash );
	explicit Highscore( const boost::filesystem::path & );
	
	bool invalid( void ) const { return !valid; }
	
	bool load( const boost::filesystem::path & );
	bool load( std::istream & );
	
	bool save( const boost::filesystem::path & ) const;
	bool save( std::ostream & ) const;
	
	void push_back_tick( int x_direction );
	
	sha256 get_level_hash( void ) const { return level_hash; }
	
	const std::string & get_player_name( void ) const { return player_name; }
	
	int get_time_ms( void ) const { return ticks * 1000 / ticks_per_second; }
	std::string get_time( void ) const { return format_time(get_time_ms()); }
	
	void set_player_name( const std::string &name ) { if (player_name.empty()) player_name = name; }
	
	static std::string format_time( int ms );
	
private:
	bool valid;
	
	std::string player_name;
	
	sha256 level_hash;
	
	int ticks_per_second;
	int ticks;
	
	struct BallControlEntry
	{
		BallControlEntry( int tick, int x_direction ) : tick(tick), x_direction(x_direction) { }
		
		const int tick;
		const int x_direction;
	};
	
	std::vector<BallControlEntry> ball_control_history;
	
	friend class Replay;
};

class Replay : public Controller
{
public:
	Replay( const Highscore & );
	
	void tick_update( void );
	
private:
	void update_down( void );
	
	const Highscore highscore;
	
	int ticks;
	
	std::vector<Highscore::BallControlEntry>::size_type ball_control_index;
};

#endif // HIGHSCORE_HPP
