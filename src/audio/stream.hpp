/*  audio/stream.hpp
 *  
 *  Copyright 2015 Carl Reinke
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
#ifndef AUDIO_STREAM_HPP
#define AUDIO_STREAM_HPP

#include "audio/channel.hpp"

#if defined(USE_TREMOR)
#include <tremor/ivorbisfile.h>
#else
#include <vorbis/vorbisfile.h>
#endif

class Stream : public Channel, boost::noncopyable
{
public:
	Stream( const std::string & );
	~Stream( void );
	
	bool empty( void ) const;
	
	void rewind( void );
	
private:
	void destroy( void );
	
	Uint8 *get_buffer( size_t &len );
	void flush( size_t len );
	
	boost::shared_array<Uint8> buffer;
	size_t size;
	
	size_t start_position;
	size_t end_position;
	bool end_of_file;
	int bitstream;
	
	OggVorbis_File *vorbis_file;
	
	SDL_AudioCVT cvt;
	
	friend class Sample;
};

#endif // AUDIO_STREAM_HPP
