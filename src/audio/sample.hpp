/*  audio/sample.hpp
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
#ifndef AUDIO_SAMPLE_HPP
#define AUDIO_SAMPLE_HPP

#include "audio/channel.hpp"
#include "audio/stream.hpp"

class Sample : public Channel
{
public:
	Sample( void );
	Sample( const std::string & );
	
	Sample( const Sample & );
	Sample( const Sample &, Fixed volume, Fixed pan );
	
	Sample & operator=( const Sample & );
	
	bool empty( void ) const;
	
private:
	void copy( const Sample & );
	
	Uint8 *get_buffer( size_t &len );
	void flush( size_t len );
	
	boost::shared_array<Uint8> buffer;
	size_t length;
	
	size_t position;
};

#endif // AUDIO_SAMPLE_HPP
