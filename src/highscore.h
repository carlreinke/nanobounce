#ifndef HIGHSCORE_H
#define HIGHSCORE_H

class Highscore
{
public:
	Highscore( int ms_per_tick );
	Highscore( std::istream & );
	
	bool operator<( const Highscore & ) const;
	
	void save( std::ostream & ) const;
	
	std::string time( void ) const;
	
	std::string name;
	std::vector<int> x_push_direction;
	
private:
	int ms() const { return ms_per_tick * x_push_direction.size(); }
	
	int ms_per_tick;
};

#endif
