#ifndef HIGHSCORE_HPP
#define HIGHSCORE_HPP

#include "controller.hpp"

class Highscore
{
public:
	Highscore( void ) { reset(); }
	Highscore( int ms_per_tick );
	bool invalid( void ) const { return ms() == 0; }
	
	bool load( const std::string &path );
	bool load( std::istream & );
	
	bool save( const std::string &path ) const;
	bool save( std::ostream & ) const;
	
	void reset( int ms_per_tick = 0 );
	
	void push_back_tick( int x_direction );
	
	int ms( void ) const { return ticks * ms_per_tick; }
	std::string time( void ) const;
	
	std::string name;
	
private:
	int ms_per_tick;
	
	int ticks;
	std::deque< std::pair<int, int> > x_direction;
	
	friend class Replay;
};

class Replay : public Controller
{
public:
	Replay( const std::string & );
	
	void tick_update( void );
	
private:
	void update_down( void );
	
	Highscore highscore;
};

#endif // HIGHSCORE_HPP
