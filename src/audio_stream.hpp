#ifndef AUDIO_STREAM_HPP
#define AUDIO_STREAM_HPP

#include "audio_channel.hpp"

#ifndef TARGET_GP2X
#include <vorbis/vorbisfile.h>
#else
#include <tremor/ivorbisfile.h>
#endif

class Stream : public Channel
{
public:
	Stream( const std::string & );
	~Stream( void );
	
	bool empty( void ) const;
	
	void rewind( void );
	
private:
	Stream( const Stream & );
	Stream & operator=( const Stream & );
	
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
