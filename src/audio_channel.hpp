#ifndef AUDIO_CHANNEL_HPP
#define AUDIO_CHANNEL_HPP

#include "fixed.hpp"
#include "SDL.h"

class Channel
{
public:
	Channel( Fixed volume = 1.0f, Fixed pan = 0.5f );
	virtual ~Channel( void ) { }
	
	Channel( const Channel & );
	
	template <class T>
	void mix_into_stream( const SDL_AudioSpec &spec, Uint8 *stream, int len, Fixed global_volume = 1 );
	
	virtual bool empty( void ) const = 0;
	
protected:
	void copy( const Channel & );
	
	virtual Uint8 *get_buffer( int &len ) = 0;
	virtual void flush( int len ) = 0;
	
	Fixed volume, pan;
};


template <class T>
void Channel::mix_into_stream( const SDL_AudioSpec &spec, Uint8 *stream_, int len, Fixed global_volume )
{
	T *stream = (T *)stream_;
	
	std::vector<Fixed> channel_volume(spec.channels, global_volume * volume);
	switch (spec.channels)
	{
	case 1:
		break;
	case 2:
	default:
		channel_volume[0] *= 1 - pan;
		channel_volume[1] *= pan;
		break;
	}
	
	int buffer_len = len;
	T *buffer = (T *)get_buffer(buffer_len);
	
	for (int i = 0; i < buffer_len / (signed)sizeof(T); )
	{
		for (int c = 0; c < spec.channels; ++c)
		{
			const Sint32 clip = stream[i] + buffer[i] * channel_volume[c];
			stream[i++] = std::min(std::max(SHRT_MIN, clip), SHRT_MAX);
		}
	}
	
	flush(buffer_len);
}

#endif // AUDIO_CHANNEL_HPP
