#include "highscore.hpp"
#include "loop.hpp"

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
	getline(is, level_path);
	getline(is, name);
	is >> ticks_per_second;
	is >> ticks;
	
	boost::trim_right_if(level_path, boost::is_any_of("\r"));
	
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
	os << level_path << endl;
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
	ticks_per_second = update_per_sec;
	
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

string Highscore::time( int ms_temp )
{
	// split milliseconds into min, sec, ms
	int ms = ms_temp % 1000;
	ms_temp -= ms;
	ms_temp /= 1000;
	int sec = ms_temp % 60;
	ms_temp -= sec;
	ms_temp /= 60;
	int min = ms_temp;
	
	ostringstream out;
	out << min << ":" << setfill('0') << setw(2) << sec << "." << setfill('0') << setw(3) << ms;
	return out.str();
}

Replay::Replay( const Highscore &score )
: ticks(0),
  highscore(score)
{
	// good to go
}

void Replay::tick_update( void )
{
	is_down[quit] = ticks > (highscore.ticks + highscore.ticks_per_second);
	
	if (!highscore.x_direction.empty() && highscore.x_direction.front().first == ticks)
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
