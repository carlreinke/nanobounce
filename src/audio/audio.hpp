/*  audio/audio.hpp
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
#ifndef AUDIO_AUDIO_HPP
#define AUDIO_AUDIO_HPP

#include "audio/sample.hpp"
#include "fixed.hpp"

#include <SDL.h>

extern void init_audio( void );
extern void deinit_audio( void );

extern void play_sample( const Sample &, Fixed volume = 1, Fixed pan = 0.5f );
extern void play_music( const std::string &path );

extern void play_next_music( void );

enum AudioMode
{
	NO_SAMPLES,
	NO_MUSIC,
	ALL_AUDIO
};

enum MusicMode
{
	NO_REPEAT,
	REPEAT_ONE,
	REPEAT_ALL
};

extern bool audio_disabled;

extern AudioMode audio_mode;
extern MusicMode music_mode;

extern Fixed volume;

extern SDL_AudioSpec spec;

extern std::list<std::string> music_paths;

inline AudioMode & operator++( AudioMode &audio_mode )
{
	audio_mode = (audio_mode == ALL_AUDIO) ? NO_SAMPLES : AudioMode(static_cast<uint>(audio_mode) + 1);
	return audio_mode;
}

#endif // AUDIO_AUDIO_HPP
