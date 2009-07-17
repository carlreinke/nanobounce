#include "controller.h"

using namespace std;

vector<Controller *> controllers, disabled_controllers;

bool Controller::drop_input_enabled = false;

Controller::Controller( void )
: is_down(functions_count, false), down_ticks(functions_count, false)
{
	if (!drop_input_enabled)
	{
		SDL_SetEventFilter(&drop_input);
		
		drop_input_enabled = true;
	}
}

int Controller::drop_input( const SDL_Event *e )
{
	switch (e->type)
	{
	case SDL_KEYDOWN:
	case SDL_KEYUP:
	case SDL_MOUSEMOTION:
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
	case SDL_JOYAXISMOTION:
	case SDL_JOYBALLMOTION:
	case SDL_JOYHATMOTION:
	case SDL_JOYBUTTONDOWN:
	case SDL_JOYBUTTONUP:
		return 0;
	default:
		return 1;
	}
}

void Controller::update( void )
{
	do_update();
	
	for (int i = 0; i < functions_count; ++i)
	{
		if (is_down[i])
		{
			if (down_ticks[i] == 0)
				push_function_event((functions)i);
			++down_ticks[i] %= down_repeat_ticks;
		}
		else if (down_ticks[i] != 0)
		{
			push_function_event((functions)i);
			down_ticks[i] = 0;
		}
	}
}

void Controller::push_function_event( functions function ) const
{
	SDLKey keys[functions_count] = 
	{
		SDLK_UP,
		SDLK_RIGHT,
		SDLK_DOWN,
		SDLK_LEFT,
		
		SDLK_SPACE,
		SDLK_BACKQUOTE,
		
		SDLK_RETURN,
		SDLK_ESCAPE,
		
		SDLK_PLUS,
		SDLK_MINUS
	};
	
	SDL_Event event;
	event.type = is_down[function] ? SDL_KEYDOWN : SDL_KEYUP;
	event.key.type = event.type;
	event.key.state = is_down[function] ? SDL_PRESSED : SDL_RELEASED;
	event.key.keysym.scancode = 0;
	event.key.keysym.sym = keys[function];
	event.key.keysym.mod = KMOD_NONE;
	event.key.keysym.unicode = 0;
	
	SDL_PushEvent(&event);
}

Keyboard::Keyboard( void )
{
	key_state = SDL_GetKeyState(NULL);
}

void Keyboard::do_update( void )
{
	is_down[up] = key_state[SDLK_UP];
	is_down[right] = key_state[SDLK_RIGHT];
	is_down[down] = key_state[SDLK_DOWN];
	is_down[left] = key_state[SDLK_LEFT];
	
	is_down[select] = key_state[SDLK_SPACE];
	is_down[back] = key_state[SDLK_BACKQUOTE];
	
	is_down[start] = key_state[SDLK_RETURN];
	is_down[quit] = key_state[SDLK_ESCAPE];
	
	is_down[vol_up] = key_state[SDLK_PLUS] || key_state[SDLK_EQUALS];
	is_down[vol_down] = key_state[SDLK_MINUS] || key_state[SDLK_UNDERSCORE];
}

Joystick::Joystick( int j )
: joystick(NULL)
{
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);
	
	if (j >= SDL_NumJoysticks())
	{
		cerr << "error: joystick " << j << " does not exist" << endl;
		return;
	}
	
	joystick = SDL_JoystickOpen(j);
	
	if (joystick == NULL)
	{
		cerr << "error: joystick " << j << " failed: " << SDL_GetError() << endl;
		return;
	}
	
	cout << "joystick '" << SDL_JoystickName(j) << "' ("
	     << SDL_JoystickNumAxes(joystick) << " axes, "
	     << SDL_JoystickNumButtons(joystick) << " buttons)" << endl;
}

Joystick::~Joystick( void )
{
	if (joystick != NULL)
	{
		SDL_JoystickClose(joystick);
		joystick = NULL;
	}
}

void Joystick::do_update( void )
{
	if (joystick == NULL)
		return;
	
	const vector<bool> was_down = is_down;
	
#ifdef TARGET_GP2X
	is_down[left]  = SDL_JoystickGetButton(joystick, GP2X_VK_UP_LEFT) ||
	                 SDL_JoystickGetButton(joystick, GP2X_VK_LEFT) ||
	                 SDL_JoystickGetButton(joystick, GP2X_VK_DOWN_LEFT) ||
	                 SDL_JoystickGetButton(joystick, GP2X_VK_FL) ||
	                 SDL_JoystickGetButton(joystick, GP2X_VK_FA);
	is_down[right] = SDL_JoystickGetButton(joystick, GP2X_VK_UP_RIGHT) ||
	                 SDL_JoystickGetButton(joystick, GP2X_VK_RIGHT) ||
	                 SDL_JoystickGetButton(joystick, GP2X_VK_DOWN_RIGHT) ||
	                 SDL_JoystickGetButton(joystick, GP2X_VK_FR) ||
	                 SDL_JoystickGetButton(joystick, GP2X_VK_FY);
	
	is_down[select] = SDL_JoystickGetButton(joystick, GP2X_VK_FB);
	is_down[back]   = SDL_JoystickGetButton(joystick, GP2X_VK_FX);
	
	is_down[start] = SDL_JoystickGetButton(joystick, GP2X_VK_START);
	is_down[quit]  = SDL_JoystickGetButton(joystick, GP2X_VK_START) &&
	                 SDL_JoystickGetButton(joystick, GP2X_VK_SELECT);
	
	is_down[vol_up]   = SDL_JoystickGetButton(joystick, GP2X_VK_VOL_UP);
	is_down[vol_down] = SDL_JoystickGetButton(joystick, GP2X_VK_VOL_DOWN);
#else
	is_down[left]  = SDL_JoystickGetAxis(joystick, 0) < SHRT_MIN / 2;
	is_down[right] = SDL_JoystickGetAxis(joystick, 0) > SHRT_MAX / 2;
	
	is_down[select] = SDL_JoystickGetButton(joystick, 0);
	is_down[back]   = SDL_JoystickGetButton(joystick, 1);
	
	is_down[start] = SDL_JoystickGetButton(joystick, 2);
	is_down[quit]  = SDL_JoystickGetButton(joystick, 3);
#endif
}

Highscore::Highscore( int ms_per_tick )
: ms_per_tick(ms_per_tick)
{
	// good to go
}

Highscore::Highscore( istream &is )
{
	getline(is, name);
	is >> ms_per_tick;
	
	for ( ; ; )
	{
		int temp;
		is >> temp;
		
		if (is.fail())
			break;
		else
			x_push_direction.push_back(temp);
	}
}

string Highscore::time( void )
{
	int temp = ms_per_tick * x_push_direction.size();
	int ms = temp % 1000;
	temp -= ms;
	temp /= 1000;
	int sec = temp % 60;
	temp -= sec;
	temp /= 60;
	int min = temp;
	
	ostringstream out;
	out << min << ":" << setw(2) << sec << "." << setw(3) << ms;
	return out.str();
}
