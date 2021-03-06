/*  audio/audio.cpp
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
#include "audio/audio.hpp"
#include "audio/stream.hpp"

void audio_callback( void *, Uint8 *stream, int len );

bool audio_disabled = false;

AudioMode audio_mode = ALL_AUDIO;
MusicMode music_mode = REPEAT_ALL;

Fixed volume = 1.0f;
Fixed music_volume = 1.0f;

SDL_AudioSpec spec;

std::list<std::unique_ptr<Channel>> channels;
std::unique_ptr<Stream> music;

std::list<std::string> music_paths;

#if !defined(AUDIO_QUALITY)
#define AUDIO_QUALITY 4
#endif

void init_audio( void )
{
	if (audio_disabled)
		return;
	
	spec.freq = 11025 * AUDIO_QUALITY;
	spec.format = AUDIO_S16SYS;
	spec.channels = 2;
	spec.samples = 512;
	spec.callback = &audio_callback;
	
	if (SDL_OpenAudio(&spec, NULL) == -1)
	{
		std::cerr << SDL_GetError() << std::endl;
		audio_disabled = true;
		return;
	}
	
	std::cout << "audio: " << spec.freq << " Hz, " << +spec.channels << " channels, " << spec.samples << " samples" << std::endl;
	
	SDL_PauseAudio(0);
	
	play_next_music();
}

void deinit_audio( void )
{
	SDL_CloseAudio();
}


void audio_callback( void *, Uint8 *stream, int len )
{
	// music
	if (music.get() != NULL && audio_mode != NO_MUSIC)
	{
		switch (spec.format)
		{
		case AUDIO_U8:
		case AUDIO_S8:
			music->mix_into_stream<Sint8>(spec, stream, len, volume * music_volume);
			break;
		default:
			music->mix_into_stream<Sint16>(spec, stream, len, volume * music_volume);
			break;
		}
		
		if (music->empty())
		{
			switch (music_mode)
			{
			case NO_REPEAT:
				break;
			case REPEAT_ONE:
				music->rewind();
				break;
			case REPEAT_ALL:
				play_next_music();
				break;
			}
		}
	}
	
	// channels
	for (auto channel_i = channels.begin(); channel_i != channels.end(); )
	{
		std::unique_ptr<Channel> &channel = *channel_i;
		
		switch (spec.format)
		{
		case AUDIO_U8:
		case AUDIO_S8:
			channel->mix_into_stream<Sint8>(spec, stream, len, volume);
			break;
		default:
			channel->mix_into_stream<Sint16>(spec, stream, len, volume);
			break;
		}
		
		if (channel->empty())
			channel_i = channels.erase(channel_i);
		else
			++channel_i;
	}
}


void play_sample( const Sample &sample, Fixed volume, Fixed pan )
{
	if (audio_disabled || audio_mode == NO_SAMPLES)
		return;
	
	SDL_LockAudio();
	
	channels.emplace_back(std::make_unique<Sample>(sample, volume, pan));
	
	SDL_UnlockAudio();
}

void play_music( const std::string &path )
{
	if (audio_disabled || audio_mode == NO_MUSIC)
		return;
	
	SDL_LockAudio();
	
	music = std::make_unique<Stream>(path);
	
	SDL_UnlockAudio();
}

void play_next_music( void )
{
	if (audio_disabled || audio_mode == NO_MUSIC)
		return;
	
	static auto current = music_paths.begin();
	
	// release old music stream
	music = std::unique_ptr<Stream>(nullptr);
	
	// if no music files
	if (music_paths.size() == 0)
		return;
	
	// loop list of music
	if (current == music_paths.end())
		current = music_paths.begin();
	
	play_music(*current);
	
	// if music fails to load, try next one
	if (music->empty())
	{
		current = music_paths.erase(current);
		play_next_music();
	}
	else
		++current;
}
