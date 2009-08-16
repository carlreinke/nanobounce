#include "highscore.h"

using namespace std;

Highscore::Highscore( int ms_per_tick )
: ms_per_tick(ms_per_tick)
{
	// good to go
}

Highscore::Highscore( istream &is )
{
	load(is);
}

void Highscore::load( istream &is )
{
	getline(is, name);
	is >> ms_per_tick;
	
	unsigned int count;
	is >> count;
	
	x_direction.clear();
	while (x_direction.size() < count)
	{
		int temp;
		is >> temp;
		x_direction.push_back(temp);
		
		if (is.fail())
			break;
	}
	
	if (is.fail())
		cout << "warning: score failed to load" << endl;
	else
		cout << "loaded score '" << name << "' " << ms() << " ms" << endl;
}

void Highscore::save( ostream &os ) const
{
	os << name << endl;
	os << ms_per_tick << " ";
	
	os << x_direction.size() << " ";
	
	for (vector<int>::const_iterator i = x_direction.begin(); i != x_direction.end(); ++i)
		os << *i << " ";
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
	out << min << ":" << setfill('0') << setw(2) << sec << "." << setfill('0') << setw(3) << ms;
	return out.str();
}
