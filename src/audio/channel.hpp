/*  audio/channel.hpp
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
 *  program; if not, see <http://www.nothinglost.net/licenses/MAME.txt>.
 */
#ifndef AUDIO_CHANNEL_HPP
#define AUDIO_CHANNEL_HPP

#include "fixed.hpp"
#include "SDL.h"

extern bool reverse_stereo;

class Channel
{
public:
	Channel( Fixed volume = 1.0f, Fixed pan = 0.5f );
	virtual ~Channel( void ) { }
	
	Channel( const Channel & );
	
	template <class T>
	void mix_into_stream( const SDL_AudioSpec &spec, Uint8 *stream, uint len, Fixed global_volume = 1 );
	
	virtual bool empty( void ) const = 0;
	
protected:
	void copy( const Channel & );
	
	virtual Uint8 *get_buffer( uint &len ) = 0;
	virtual void flush( uint len ) = 0;
	
	Fixed volume, pan;
};


template <typename T>
void Channel::mix_into_stream( const SDL_AudioSpec &spec, Uint8 *stream_, uint len, Fixed global_volume )
{
	if (empty())
		return;
	
	T *stream = reinterpret_cast<T *>(stream_);
	
	std::vector<Fixed> channel_volume(spec.channels, global_volume * volume);
	switch (spec.channels)
	{
	case 1:
		break;
	case 2:
	default:
		channel_volume[0] *= 1 - pan;
		channel_volume[1] *= pan;
		
		if (reverse_stereo)
			std::swap(channel_volume[0], channel_volume[1]);
		
		break;
	}
	
	T *buffer = reinterpret_cast<T *>(get_buffer(len));
	
	for (uint i = len / (sizeof(T) * spec.channels); i > 0; --i)
	{
		for (uint c = 0; c < spec.channels; ++c)
		{
			const Sint32 clip = *stream + static_cast<Sint32>(*(buffer++) * channel_volume[c]);
			*(stream++) = std::min(std::max(static_cast<Sint32>(std::numeric_limits<T>::min()), clip), static_cast<Sint32>(std::numeric_limits<T>::max()));
		}
	}
	
	flush(len);
}

#endif // AUDIO_CHANNEL_HPP
