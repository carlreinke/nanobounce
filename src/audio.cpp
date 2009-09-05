#include "audio.hpp"

using namespace std;

void audio_callback( void *, Uint8 *stream, int len );
template <class T>
void audio_mixer( Uint8 *stream_, int len );

Fixed volume = 0.5f;

SDL_AudioSpec spec;

vector<Channel *> channels;

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
}

void deinit_audio( void )
{
	SDL_CloseAudio();
}

void audio_callback( void *, Uint8 *stream, int len )
{
	switch (spec.format)
	{
	case AUDIO_U8:
	case AUDIO_S8:
		audio_mixer<Sint8>(stream, len);
		break;
	default:
		audio_mixer<Sint16>(stream, len);
		break;
	}
}

template <class T>
void audio_mixer( Uint8 *stream_, int len )
{
	T *stream = (T *)stream_;
	
	for (vector<Channel *>::iterator channel_i = channels.begin(); channel_i != channels.end(); )
	{
		Channel *channel = *channel_i;
		
		const Fixed pan = (spec.channels == 1) ? (Fixed)0 : channel->pan;
		vector<Fixed> stereo_volume;
		stereo_volume.push_back(volume * channel->volume * (1 - pan));
		stereo_volume.push_back(volume * channel->volume * pan);
		
		int buffer_len = len;
		T *buffer = (T *)channel->get_buffer(buffer_len);
		
		for (int i = 0; i < buffer_len / (signed)sizeof(T); )
		{
			for (int c = 0; c < spec.channels; ++c)
			{
				const Sint32 clip = stream[i] + buffer[i] * stereo_volume[c];
				stream[i++] = min(max(SHRT_MIN, clip), SHRT_MAX);
			}
		}
		
		channel->flush(buffer_len);
		
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
	if (audio_disabled)
		return;
	
	SDL_LockAudio();
	
	channels.push_back(new Sample(sample, volume, pan));
	
	SDL_UnlockAudio();
}
