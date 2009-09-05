#ifndef AUDIO_HPP
#define AUDIO_HPP

#include "audio_sample.hpp"
#include "fixed.hpp"
#include "SDL.h"

extern Fixed volume;

extern SDL_AudioSpec spec;

extern std::map<std::string, Sample> samples;

void init_audio( void );
void deinit_audio( void );

void play_sample( const Sample &, Fixed volume, Fixed pan );

#endif // AUDIO_HPP
