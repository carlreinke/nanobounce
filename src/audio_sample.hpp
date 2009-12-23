#ifndef AUDIO_SAMPLE_HPP
#define AUDIO_SAMPLE_HPP

#include "audio_channel.hpp"

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
	
	Uint8 *get_buffer( uint &len );
	void flush( uint len );
	
	boost::shared_array<Uint8> buffer;
	uint length;
	
	uint position;
};

#endif // AUDIO_SAMPLE_HPP
