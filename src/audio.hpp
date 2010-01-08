#ifndef AUDIO_HPP
#define AUDIO_HPP

#include "audio_sample.hpp"
#include "fixed.hpp"
#include "SDL.h"

enum AudioMode
{
	NO_SAMPLES,
	NO_MUSIC,
	ALL_AUDIO
};

extern AudioMode audio_mode;
extern Fixed volume;

extern SDL_AudioSpec spec;

extern std::map<std::string, Sample> samples;

extern bool audio_disabled;

void init_audio( void );
void deinit_audio( void );

void play_sample( const Sample &, Fixed volume, Fixed pan );
void play_music( const std::string &path );


inline AudioMode & operator++( AudioMode &audio_mode )
{
	if (audio_mode == ALL_AUDIO)
		audio_mode = NO_SAMPLES;
	else
		audio_mode = AudioMode(static_cast<uint>(audio_mode) + 1);
	return audio_mode;
}

#endif // AUDIO_HPP
