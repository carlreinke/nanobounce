#include "audio.hpp"

using namespace std;

Fixed volume = 0.5f;

SDL_AudioSpec spec;

vector<Stream> streams;

std::map<std::string, Sample> samples;

static bool audio_disabled = false;

Stream::Stream( const Sample &sample, Fixed volume, Fixed pan )
: sample(sample), position(0),
  volume(min(max((Fixed)0, volume), (Fixed)1)), pan(min(max((Fixed)0, pan), (Fixed)1))
{
	// good to go
}

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

void audio_callback( void *, Uint8 *stream_, int len )
{
	Uint16 *stream = (Uint16 *)stream_;
	len /= 2; // because 16 bit
	
	for (vector<Stream>::iterator s = streams.begin(); s != streams.end(); )
	{
		Fixed pan = (spec.channels == 1) ? (Fixed)0 : s->pan;
		vector<Fixed> stereo_volume;
		stereo_volume.push_back(volume * s->volume * (1 - pan));
		stereo_volume.push_back(volume * s->volume * pan);
		
		int left = (s->sample.length / 2 - s->position) * spec.channels;
		for (int i = 0; i < min(len, left); )
		{
			for (int c = 0; c < spec.channels; ++c)
			{
				Sint32 clip = stream[i] + ((Sint16 *)s->sample.buffer)[s->position] * stereo_volume[c];
				stream[i++] = min(max(SHRT_MIN, clip), SHRT_MAX);
			}
			
			++s->position;
		}
		
		if (len >= left)
			streams.erase(s);
		else
			++s;
	}
}
