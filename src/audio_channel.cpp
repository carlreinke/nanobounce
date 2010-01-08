#include "audio_channel.hpp"

using namespace std;

bool reverse_stereo = false;

Channel::Channel( Fixed volume, Fixed pan )
: volume(min(max(Fixed(0), volume), Fixed(1))), pan(min(max(Fixed(0), pan), Fixed(1)))
{
	// good to go
}

Channel::Channel( const Channel &that )
{
	copy(that);
}

void Channel::copy( const Channel &that )
{
	volume = that.volume;
	pan = that.pan;
}
