#ifndef HIGHSCORE_HPP
#define HIGHSCORE_HPP

#include "controller/controller.hpp"

class Highscore
{
public:
	Highscore( void ) { reset(); }
	explicit Highscore( const std::string &score_path ) { load(score_path); }
	bool invalid( void ) const { return ms() == 0; }
	
	bool load( const std::string &path );
	bool load( std::istream & );
	
	bool save( void ) const { return save(level_path + ".score"); };
	bool save( const std::string &path ) const;
	bool save( std::ostream & ) const;
	
	void reset( void );
	
	void push_back_tick( int x_direction );
	
	int ms( void ) const { return ticks * 1000 / ticks_per_second; }
	std::string time( void ) const { return time(ms()); }
	
	std::string name;
	std::string level_path;
	
	static std::string time( int ms );
	
private:
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
