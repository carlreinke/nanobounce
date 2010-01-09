#include "audio.hpp"
#include "audio_sample.hpp"

using namespace std;

Sample::Sample( void )
: Channel(),
  buffer(NULL), length(0), position(0)
{
	// good to go
}

Sample::Sample( const string &path )
: Channel(),
  buffer(NULL), length(0), position(0)
{
	Stream stream(path);
	
	if (stream.empty())
		return;
	
	length = sizeof(Sint16) * spec.channels * ov_pcm_total(stream.vorbis_file, -1);
	buffer = boost::shared_array<Uint8>(new Uint8[length]);
	
	// copy entire stream into buffer
	while (position < length && !stream.empty())
	{
		uint amount = min(stream.size, length - position);
		memcpy(&buffer[position], stream.get_buffer(amount), amount);
		stream.flush(amount);
		position += amount;
	}
	
	position = 0;
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


Uint8 * Sample::get_buffer( uint &len )
{
	len = min(len, length - position);
	return &buffer[position];
}

void Sample::flush( uint len )
{
	position += len;
}

bool Sample::empty( void ) const
{
	return (position == length);
}


void Sample::copy( const Sample &that )
{
	buffer = that.buffer;
	length = that.length;
	
	position = 0;
}
