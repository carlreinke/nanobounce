#include "controller.hpp"

using namespace std;

const SDLKey Controller::push_as_key[] = 
{
	static_cast<SDLKey>(up_key),
	static_cast<SDLKey>(right_key),
	static_cast<SDLKey>(down_key),
	static_cast<SDLKey>(left_key),
	
	static_cast<SDLKey>(left_shoulder_key),
	static_cast<SDLKey>(right_shoulder_key),
	
	static_cast<SDLKey>(select_key),
	static_cast<SDLKey>(back_key),
	
	static_cast<SDLKey>(start_key),
	static_cast<SDLKey>(quit_key),
	
	static_cast<SDLKey>(vol_up_key),
	static_cast<SDLKey>(vol_down_key),
};
const Uint32 Controller::repeat_delay, Controller::repeat_interval;

vector<Controller *> controllers, disabled_controllers;

bool Controller::drop_input_enabled = false;

Controller::Controller( void )
: is_down(functions_count, false), repeat_tick(functions_count, false)
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
	update_down();
	
	const Uint32 tick = SDL_GetTicks();
	
	for (int i = 0; i < functions_count; ++i)
	{
		if (is_down[i])
		{
			if (tick > repeat_tick[i])
			{
				push_function_event(Functions(i));
				if (repeat_tick[i] == 0)  // if pressed
					repeat_tick[i] = tick + repeat_delay;
				else                      // if held
					repeat_tick[i] += repeat_interval;
			}
		}
		else if (repeat_tick[i] != 0)
		{
			push_function_event(Functions(i));
			repeat_tick[i] = 0;
		}
	}
}

void Controller::push_function_event( Functions function ) const
{
	SDL_Event event;
	event.type = is_down[function] ? SDL_KEYDOWN : SDL_KEYUP;
	event.key.type = event.type;
	event.key.state = is_down[function] ? SDL_PRESSED : SDL_RELEASED;
	event.key.keysym.scancode = 0;
	event.key.keysym.sym = push_as_key[function];
	event.key.keysym.mod = KMOD_NONE;
	event.key.keysym.unicode = 0;
	
	SDL_PushEvent(&event);
}

Keyboard::Keyboard( void )
{
	key_state = SDL_GetKeyState(NULL);
}

void Keyboard::update_down( void )
{
	is_down[left]  = key_state[left_key];
	is_down[right] = key_state[right_key];
	
	is_down[left_shoulder]  = key_state[left_shoulder_key];
	is_down[right_shoulder] = key_state[right_shoulder_key];
	
	is_down[up]   = key_state[up_key];
	is_down[down] = key_state[down_key];
	
	is_down[select] = key_state[select_key];
	is_down[back]   = key_state[back_key];
	
	is_down[start] = key_state[start_key];
	is_down[quit]  = key_state[quit_key];
	
	is_down[vol_up]   = key_state[vol_up_key] || key_state[SDLK_EQUALS];
	is_down[vol_down] = key_state[vol_down_key] || key_state[SDLK_UNDERSCORE];
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

void Joystick::update_down( void )
{
	if (joystick == NULL)
		return;
	
#ifdef TARGET_GP2X
	is_down[left]  = SDL_JoystickGetButton(joystick, GP2X_VK_UP_LEFT) ||
	                 SDL_JoystickGetButton(joystick, GP2X_VK_LEFT) ||
	                 SDL_JoystickGetButton(joystick, GP2X_VK_DOWN_LEFT);
	is_down[right] = SDL_JoystickGetButton(joystick, GP2X_VK_UP_RIGHT) ||
	                 SDL_JoystickGetButton(joystick, GP2X_VK_RIGHT) ||
	                 SDL_JoystickGetButton(joystick, GP2X_VK_DOWN_RIGHT)
	
	is_down[left_shoulder]  = SDL_JoystickGetButton(joystick, GP2X_VK_FL);
	is_down[right_shoulder] = SDL_JoystickGetButton(joystick, GP2X_VK_FR);
	
	is_down[up]   = SDL_JoystickGetButton(joystick, GP2X_VK_UP_LEFT) ||
	                SDL_JoystickGetButton(joystick, GP2X_VK_UP) ||
	                SDL_JoystickGetButton(joystick, GP2X_VK_UP_RIGHT);
	is_down[down] = SDL_JoystickGetButton(joystick, GP2X_VK_DOWN_LEFT) ||
	                SDL_JoystickGetButton(joystick, GP2X_VK_DOWN) ||
	                SDL_JoystickGetButton(joystick, GP2X_VK_DOWN_RIGHT);
	
	is_down[select] = SDL_JoystickGetButton(joystick, GP2X_VK_FB);
	is_down[back]   = SDL_JoystickGetButton(joystick, GP2X_VK_FX);
	
	is_down[start] = SDL_JoystickGetButton(joystick, GP2X_VK_START);
	is_down[quit]  = SDL_JoystickGetButton(joystick, GP2X_VK_START) &&
	                 SDL_JoystickGetButton(joystick, GP2X_VK_SELECT);
	
	is_down[vol_up]   = SDL_JoystickGetButton(joystick, GP2X_VK_VOL_UP);
	is_down[vol_down] = SDL_JoystickGetButton(joystick, GP2X_VK_VOL_DOWN);
#else
	is_down[left]  = SDL_JoystickGetAxis(joystick, 0) < numeric_limits<Sint16>::min() / 2;
	is_down[right] = SDL_JoystickGetAxis(joystick, 0) > numeric_limits<Sint16>::max() / 2;
	
	is_down[up]   = SDL_JoystickGetAxis(joystick, 1) < numeric_limits<Sint16>::min() / 2;
	is_down[down] = SDL_JoystickGetAxis(joystick, 1) > numeric_limits<Sint16>::max() / 2;
	
	is_down[select] = SDL_JoystickGetButton(joystick, 0);
	is_down[back]   = SDL_JoystickGetButton(joystick, 1);
	
	is_down[start] = SDL_JoystickGetButton(joystick, 2);
	is_down[quit]  = SDL_JoystickGetButton(joystick, 3);
#endif
}
