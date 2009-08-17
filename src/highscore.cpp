#include "highscore.h"

using namespace std;

Highscore::Highscore( int ms_per_tick )
: ms_per_tick(ms_per_tick), ticks(0)
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
	is >> ticks;
	
	if (name.size() == 0)
		name = "UNKNOWN";
	
	x_direction.clear();
	for (; ; )
	{
		int tick, x_direction;
		is >> tick >> x_direction;
		
		if (is.fail() || tick == ticks)
			break;
		
		this->x_direction.push_back(make_pair(tick, x_direction));
	}
	
	if (is.fail())
		reset();
	
	if (invalid())
		cout << "warning: score failed to load" << endl;
	else
		cout << "loaded score '" << name << "' " << ms() << " ms" << endl;
}

void Highscore::save( ostream &os ) const
{
	os << name << endl;
	os << ms_per_tick << " ";
	os << ticks << " ";
	
	for (vector< pair<int, int> >::const_iterator i = x_direction.begin(); i != x_direction.end(); ++i)
		os << i->first << " " << i->second << " ";
	os << ticks << " " << 0 << endl;
}

void Highscore::reset( int ms_per_tick )
{
	this->ms_per_tick = ms_per_tick;
	ticks = 0;
	x_direction.clear();
}

void Highscore::push_back_tick( int x_direction )
{
	if (this->x_direction.size() == 0 || this->x_direction.back().second != x_direction)
		this->x_direction.push_back(make_pair(ticks, x_direction));
	
	++ticks;
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
