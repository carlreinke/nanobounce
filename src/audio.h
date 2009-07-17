#ifndef AUDIO_H
#define AUDIO_H

#include "audio_sample.h"
#include "fixed.h"
#include "SDL.h"

class Stream
{
public:
	Stream( const Sample &, Fixed volume = 1.0f, Fixed pan = 0.5f );
	
private:
	Sample sample;
	unsigned int position;
	
	Fixed volume, pan;
	
	friend void audio_callback( void *, Uint8 *stream, int len );
};

extern Fixed volume;

extern SDL_AudioSpec spec;

extern std::vector<Stream> streams;

extern std::map<std::string, Sample> samples;

void init_audio( void );
void deinit_audio( void );

void audio_callback( void *, Uint8 *stream, int len );

#endif
