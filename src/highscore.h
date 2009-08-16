#ifndef HIGHSCORE_H
#define HIGHSCORE_H

class Highscore
{
public:
	Highscore( void ) { }
	Highscore( int ms_per_tick );
	Highscore( std::istream & );
	
	void load( std::istream & );
	void save( std::ostream & ) const;
	
	int ms( void ) const { return ms_per_tick * x_direction.size(); }
	std::string time( void ) const;
	
	bool empty( void ) const { return ms() == 0 || name.size() == 0; }
	
	std::string name;
	std::vector<int> x_direction;
	
private:
	int ms_per_tick;
};

#endif
