/*  audio/sample.cpp
 *  
 *  Copyright 2010 Carl Reinke
 *  
 *  This program is non-commercial, open-source software; you can redistribute
 *  it and/or modify it under the terms of the MAME License as included along
 *  with this program.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  MAME License for more details.
 *  
 *  You should have received a copy of the MAME License along with this
 *  program; if not, see <http://www.intoorbit.org/licenses/MAME.txt>.
 */
#include "audio/audio.hpp"
#include "audio/sample.hpp"

using namespace std;

Sample::Sample( void )
: Channel(),
  buffer(), length(0), position(0)
{
	// good to go
}

Sample::Sample( const string &path )
: Channel(),
  buffer(), length(0), position(0)
{
	Stream stream(path);
	
	if (stream.empty())
		return;
	
	length = sizeof(Sint16) * ov_pcm_total(stream.vorbis_file, -1) * stream.cvt.len_ratio;
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

Uint8 * Sample::get_buffer( uint &len )
{
	len = min(len, length - position);
	return &buffer[position];
}

void Sample::flush( uint len )
{
	position += len;
}
