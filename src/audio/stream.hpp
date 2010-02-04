/*  audio/stream.hpp
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
 *  program; if not, see <http://www.nothinglost.net/licenses/MAME.txt>.
 */
#ifndef AUDIO_STREAM_HPP
#define AUDIO_STREAM_HPP

#include "audio/channel.hpp"

#ifndef TARGET_GP2X
#include <vorbis/vorbisfile.h>
#else
#include <tremor/ivorbisfile.h>
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
	
	Uint8 *get_buffer( uint &len );
	void flush( uint len );
	
	boost::shared_array<Uint8> buffer;
	uint size;
	
	uint start_position, end_position;
	bool end_of_file;
	int bitstream;
	
	OggVorbis_File *vorbis_file;
	
	SDL_AudioCVT cvt;
	
	friend class Sample;
};

#endif // AUDIO_STREAM_HPP
