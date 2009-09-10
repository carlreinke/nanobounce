#include "audio.hpp"
#include "audio_stream.hpp"

using namespace std;

void audio_callback( void *, Uint8 *stream, int len );

Audio_mode audio_mode = ALL_AUDIO;
Fixed volume = 0.5f, music_volume = 0.75f;

SDL_AudioSpec spec;

vector<Channel *> channels;
auto_ptr<Stream> music;

std::map<std::string, Sample> samples;

static bool audio_disabled = false;

void init_audio( void )
{
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
	
	cout << "audio: " << spec.freq << " Hz, " << (int)spec.channels << " channels, " << spec.samples << " samples" << endl;
	
	SDL_PauseAudio(0);
	
	samples["bounce"] = Sample("smp/bounce.wav");
	samples["wall_jump"] = Sample("smp/wall_jump.wav");
	samples["recycle"] = Sample("smp/recycle.wav");
	samples["explode"] = Sample("smp/explode.wav");
	samples["boost"] = Sample("smp/boost.wav");
	samples["unboost"] = Sample("smp/unboost.wav");
	
	samples["won"] = Sample("smp/won.wav");
	samples["lost"] = Sample("smp/lost.wav");
	
	music = auto_ptr<Stream>(new Stream("music/01 Papilio.ogg"));
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
			music->rewind();
	}
	
	// channels
	for (vector<Channel *>::iterator channel_i = channels.begin(); channel_i != channels.end(); )
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
			channels.erase(channel_i);
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
