#include "highscore.h"

using namespace std;

Highscore::Highscore( int ms_per_tick )
: ms_per_tick(ms_per_tick)
{
	// good to go
}

Highscore::Highscore( istream &is )
{
	getline(is, name);
	is >> ms_per_tick;
	
	for ( ; ; )
	{
		int temp;
		is >> temp;
		
		if (is.fail())
			break;
		else
			x_push_direction.push_back(temp);
	}
}

bool Highscore::operator<( const Highscore &that ) const
{
	return ms() < that.ms();
}

string Highscore::time( void ) const
{
	// split milliseconds into min, sec, ms
	int temp = ms();
	int ms = temp % 1000;
	temp -= ms;
	temp /= 1000;
	int sec = temp % 60;
	temp -= sec;
	temp /= 60;
	int min = temp;
	
	ostringstream out;
	out << min << ":" << setw(2) << sec << "." << setw(3) << ms;
	return out.str();
}
