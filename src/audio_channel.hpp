#ifndef AUDIO_CHANNEL_HPP
#define AUDIO_CHANNEL_HPP

#include "fixed.hpp"
#include "SDL.h"

class Channel
{
public:
	Channel( Fixed volume = 1.0f, Fixed pan = 0.5f );
	
	Channel( const Channel & );
	
	virtual Uint8 *get_buffer( int &len ) = 0;
	virtual void flush( int len ) = 0;
	virtual bool empty( void ) const = 0;
	
protected:
	void copy( const Channel & );
	
	Fixed volume, pan;
	
	template <class T>
	friend void audio_mixer( Uint8 *stream, int len );
};

#endif // AUDIO_CHANNEL_HPP
