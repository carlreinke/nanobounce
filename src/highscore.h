#ifndef HIGHSCORE_H
#define HIGHSCORE_H

class Highscore
{
public:
	Highscore( int ms_per_tick );
	Highscore( std::istream & );
	
	bool operator<( const Highscore & ) const;
	
	void load( std::istream & );
	void save( std::ostream & ) const;
	
	std::string time( void ) const;
	
	std::string name;
	std::vector<int> x_direction;
	
private:
	int ms() const { return ms_per_tick * x_direction.size(); }
	
	int ms_per_tick;
};

#endif
