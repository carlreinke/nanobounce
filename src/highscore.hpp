#ifndef HIGHSCORE_HPP
#define HIGHSCORE_HPP

#include "controller.hpp"

class Highscore
{
public:
	Highscore( void ) { reset(); }
	Highscore( int ms_per_tick );
	Highscore( std::istream & );
	
	void load( std::istream & );
	void save( std::ostream & ) const;
	void reset( int ms_per_tick = 0 );
	
	void push_back_tick( int x_direction );
	
	int ms( void ) const { return ticks * ms_per_tick; }
	std::string time( void ) const;
	
	bool invalid( void ) const { return ms() == 0; }
	
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
	Replay( std::istream & );
	Replay( const std::string & );
	
private:
	void do_update( void );
	
	void load( std::istream & );
	
	Highscore highscore;
};

#endif // HIGHSCORE_HPP
