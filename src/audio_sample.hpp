#ifndef AUDIO_SAMPLE_HPP
#define AUDIO_SAMPLE_HPP

#include "SDL.h"

class Sample
{
public:
	Sample( void );
	Sample( const char *file );
	Sample( const Sample &  );
	~Sample( void );
	
	Sample & operator=( const Sample & );
	
private:
	void copy( const Sample & );
	void destroy( void );
	
	Uint8 *buffer;
	unsigned int length;
	
	friend void audio_callback( void *, Uint8 *stream, int len );
};

#endif // AUDIO_SAMPLE_HPP
