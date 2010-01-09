#include "highscore.hpp"
#include "main.hpp"

using namespace std;

bool Highscore::load( const string &data_path )
{
	ifstream data(data_path.c_str());
	
	bool success = load(data);
	
	if (success)
		cout << "loaded score '" << name << "' " << ms() << " ms from '" << data_path << "'" << endl;
	else
		cout << "warning: failed to load score from '" << data_path << "'" << endl;
	
	return success;
}

bool Highscore::load( istream &is )
{
	getline(is, name);
	is >> ticks_per_second;
	is >> ticks;
	
	if (name.size() == 0)
		name = "UNKNOWN";
	
	x_direction.clear();
	for (; ; )
	{
		int tick, x_direction;
		is >> tick >> x_direction;
		
		if (!is.good() || tick == ticks)
			break;
		
		this->x_direction.push_back(make_pair(tick, x_direction));
	}
	
	if (!is.good()) // highscore file corrupt?
		reset();
	
	return !invalid();
}

bool Highscore::save( const string &data_path ) const
{
	ofstream data(data_path.c_str());
	
	bool success = save(data);
	
	cout << (success ? "saved" : "warning: failed to save") << " score"
	     << " '" << name << "' " << ms() << " ms to '" << data_path << "'" << endl;
	
	return success;
}

bool Highscore::save( ostream &os ) const
{
	os << name << endl;
	os << ticks_per_second << " ";
	os << ticks << " ";
	
	for (deque< pair<int, int> >::const_iterator i = x_direction.begin(); i != x_direction.end(); ++i)
		os << i->first << " " << i->second << " ";
	os << ticks << " " << 0 << endl;
	
	return os.good();
}

void Highscore::reset( void )
{
	ticks_per_second = ups * ups_multiplier;
	
	ticks = 0;
	x_direction.clear();
}

void Highscore::push_back_tick( int x_direction )
{
	// if direction different than last tick, store it
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

Replay::Replay( const string &data_path )
{
	highscore.load(data_path);
}

void Replay::tick_update( void )
{
	static int ticks = 0;
	
	if (ticks > highscore.ticks || highscore.x_direction.empty())
		return;
	
	if (highscore.x_direction.front().first == ticks)
	{
		switch (highscore.x_direction.front().second)
		{
		case -1:
			is_down[left] = true;
			is_down[right] = false;
			break;
		case 0:
			is_down[left] = false;
			is_down[right] = false;
			break;
		case 1:
			is_down[left] = false;
			is_down[right] = true;
			break;
		default:
			break;
		}
		
		highscore.x_direction.pop_front();
	}
	
	++ticks;
}

void Replay::update_down( void )
{
	// nothing to do
}
