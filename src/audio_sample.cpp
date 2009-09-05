#include "audio.hpp"
#include "audio_sample.hpp"

using namespace std;

Sample::Sample( void )
: Channel(),
  buffer(NULL), size(0), position(0)
{
	// good to go
}

Sample::Sample( const string &path )
: Channel()
{
	(void)path;
	clog << "TODO " << __PRETTY_FUNCTION__ << endl;
}

Sample::Sample( const Sample &that )
: Channel(that)
{
	copy(that);
}

Sample::Sample( const Sample &that, Fixed volume, Fixed pan )
: Channel(that)
{
	copy(that);
	
	this->volume = volume;
	this->pan = pan;
}

Sample & Sample::operator=( const Sample &that )
{
	if (this != &that)
	{
		Channel::operator=( that );
		copy(that);
	}
	return *this;
}


Uint8 * Sample::get_buffer( int &len )
{
	clog << "TODO " << __PRETTY_FUNCTION__ << endl;
	len = 0;
	return NULL;
}

void Sample::flush( int len )
{
	(void)len;
	clog << "TODO " << __PRETTY_FUNCTION__ << endl;
}

bool Sample::empty( void ) const
{
	return (position == size);
}


void Sample::copy( const Sample &that )
{
	buffer = that.buffer;
	size = that.size;
	
	position = 0;
}
