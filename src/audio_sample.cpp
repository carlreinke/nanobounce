#include "audio.hpp"
#include "audio_sample.hpp"

using namespace std;

Sample::Sample( void )
: Channel(),
  buffer(NULL), length(0), position(0)
{
	// good to go
}

Sample::Sample( const string &path )
: Channel(),
  position(0)
{
	SDL_AudioSpec wav_spec;
	SDL_AudioCVT wav_cvt;
	Uint8 *wav_buffer;
	Uint32 wav_length;
	
	if (SDL_LoadWAV(path.c_str(), &wav_spec, &wav_buffer, &wav_length) == NULL)
	{
		cerr << "error: '" << path << "' failed to load: " << SDL_GetError() << endl;
		return;
	}
	
	if (SDL_BuildAudioCVT(&wav_cvt,
	                      wav_spec.format, wav_spec.channels, wav_spec.freq,
	                      spec.format,     spec.channels,     spec.freq) == -1)
	{
		cerr << "error: '" << path << "' failed to load: " << SDL_GetError() << endl;
		return;
	}
	
	wav_cvt.len = wav_length;
	wav_cvt.buf = new Uint8[wav_cvt.len * wav_cvt.len_mult];
	
	memcpy(wav_cvt.buf, wav_buffer, wav_length);
	
	SDL_FreeWAV(wav_buffer);
	
	SDL_ConvertAudio(&wav_cvt);
	
	length = wav_cvt.len_cvt;
	buffer = boost::shared_array<Uint8>(wav_cvt.buf);
}

Sample::Sample( const Sample &that )
: Channel(that)
{
	copy(that);
}

Sample::Sample( const Sample &that, Fixed volume, Fixed pan )
: Channel(that)
{
	copy(that);
	
	this->volume = volume;
	this->pan = pan;
}

Sample & Sample::operator=( const Sample &that )
{
	if (this != &that)
	{
		Channel::operator=( that );
		copy(that);
	}
	return *this;
}


Uint8 * Sample::get_buffer( int &len )
{
	len = min(len, length - position);
	return &buffer[position];
}

void Sample::flush( int len )
{
	position += len;
}

bool Sample::empty( void ) const
{
	return (position == length);
}


void Sample::copy( const Sample &that )
{
	buffer = that.buffer;
	length = that.length;
	
	position = 0;
}
