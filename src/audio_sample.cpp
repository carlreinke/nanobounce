#include "audio.hpp"
#include "audio_sample.hpp"

using namespace std;

Sample::Sample()
: buffer(NULL), length(0)
{
	// good to do
}

Sample::Sample( const char *file )
: buffer(NULL), length(0)
{
	SDL_AudioSpec wav_spec;
	SDL_AudioCVT wav_cvt;
	Uint8 *wav_buffer;
	Uint32 wav_length;
	
	if (SDL_LoadWAV(file, &wav_spec, &wav_buffer, &wav_length) == NULL)
	{
		cerr << "error: '" << file << "' failed to load: " << SDL_GetError() << endl;
		return;
	}
	
	if (SDL_BuildAudioCVT(&wav_cvt,
	                      wav_spec.format, wav_spec.channels, wav_spec.freq,
	                      spec.format, 1 /*spec.channels*/, spec.freq) == -1)
	{
		cerr << "error: '" << file << "' failed to load: " << SDL_GetError() << endl;
		return;
	}
	
	wav_cvt.len = wav_length;
	wav_cvt.buf = new Uint8[wav_cvt.len * wav_cvt.len_mult];
	
	memcpy(wav_cvt.buf, wav_buffer, wav_length);
	
	SDL_FreeWAV(wav_buffer);
	
	SDL_ConvertAudio(&wav_cvt);
	
	length = wav_cvt.len_cvt;
	buffer = wav_cvt.buf;
}

Sample::Sample( const Sample &that )
{
	copy(that);
}

Sample::~Sample( void )
{
	destroy();
}

Sample & Sample::operator=( const Sample &that )
{
	if (this != &that)
	{
		destroy();
		copy(that);
	}
	return *this;
}

void Sample::copy( const Sample &that )
{
	length = that.length;
	buffer = new Uint8[length];
	memcpy(buffer, that.buffer, length);
}

void Sample::destroy( void )
{
	delete[] buffer;
}
