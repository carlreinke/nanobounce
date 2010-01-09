#include "audio.hpp"
#include "audio_stream.hpp"
#include "file_system.hpp"
#include "main.hpp"

using namespace std;

void audio_callback( void *, Uint8 *stream, int len );

void play_next_music( void );

AudioMode audio_mode = ALL_AUDIO;
Fixed volume = 0.5f, music_volume = 1.0f;

SDL_AudioSpec spec;

list<Channel *> channels;
auto_ptr<Stream> music;

std::vector<Sample> samples;

bool audio_disabled = false;

void init_audio( void )
{
	if (audio_disabled)
		return;
	
	spec.freq = 11025 * 2;
	spec.format = AUDIO_S16SYS;
	spec.channels = 2;
	spec.samples = 256;
	spec.callback = &audio_callback;
	
	if (SDL_OpenAudio(&spec, NULL) == -1)
	{
		cerr << SDL_GetError() << endl;
		audio_disabled = true;
		return;
	}
	
	cout << "audio: " << spec.freq << " Hz, " << +spec.channels << " channels, " << spec.samples << " samples" << endl;
	
	SDL_PauseAudio(0);
	
	const pair<SampleName, string> sample_files[] =
	{
		make_pair(BOUNCE,    "bounce.ogg"),
		make_pair(WALL_JUMP, "wall_jump.ogg"),
		make_pair(RECYCLE,   "recycle.ogg"),
		make_pair(NUKE,      "nuke.ogg"),
		make_pair(BOOST,     "boost.ogg"),
		make_pair(UNBOOST,   "unboost.ogg"),
		make_pair(WON,       "won.ogg"),
		make_pair(LOST,      "lost.ogg"),
	};
	
	samples.resize(sample_max);
	for (uint i = 0; i < COUNTOF(sample_files); ++i)
		samples[sample_files[i].first] = Sample(sample_directory + sample_files[i].second);
	
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
			play_next_music();
	}
	
	// channels
	for (list<Channel *>::iterator channel_i = channels.begin(); channel_i != channels.end(); )
	{
		Channel *channel = *channel_i;
		
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
		{
			delete channel;
			channel_i = channels.erase(channel_i);
		}
		else
			++channel_i;
	}
}


void play_sample( const Sample &sample, Fixed volume, Fixed pan )
{
	if (audio_disabled || audio_mode == NO_SAMPLES)
		return;
	
	SDL_LockAudio();
	
	channels.push_back(new Sample(sample, volume, pan));
	
	SDL_UnlockAudio();
}

void play_music( const std::string &path )
{
	if (audio_disabled || audio_mode == NO_MUSIC)
		return;
	
	music = auto_ptr<Stream>(new Stream(path));
}

void play_next_music( void )
{
	if (audio_disabled || audio_mode == NO_MUSIC)
		return;
	
	static vector<string> entries = directory_listing(music_directory);
	static vector<string>::iterator entry = entries.begin();
	
	music = auto_ptr<Stream>(NULL);
	
	// no music files?
	if (entries.size() == 0)
		return;
	
	if (entry == entries.end())
		entry = entries.begin();
	
	play_music(music_directory + *entry);
	
	// if music fails to load, try next one
	if (music->empty())
	{
		entry = entries.erase(entry);
		play_next_music();
	}
	else
		++entry;
}
