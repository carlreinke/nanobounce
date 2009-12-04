#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "SDL.h"

class Controller
{
public:
	Controller( void );
	virtual ~Controller( void ) { }
	
	void update( void );
	virtual void tick_update( void ) { };
	
	virtual bool is_keyboard( void ) { return false; }
	
	enum Functions
	{
		up = 0,
		right,
		down,
		left,
		
		left_shoulder,
		right_shoulder,
		
		select,
		back,
		
		start,
		quit,
		
		vol_up,
		vol_down,
		
		functions_count
	};
	enum Keys
	{
		up_key             = SDLK_UP,
		right_key          = SDLK_RIGHT,
		down_key           = SDLK_DOWN,
		left_key           = SDLK_LEFT,
		
		left_shoulder_key  = SDLK_LEFTBRACKET,
		right_shoulder_key = SDLK_RIGHTBRACKET,
		
		select_key         = SDLK_SPACE,
		back_key           = SDLK_BACKQUOTE,
		
		start_key          = SDLK_RETURN,
		quit_key           = SDLK_ESCAPE,
		
		vol_up_key         = SDLK_PLUS,
		vol_down_key       = SDLK_MINUS
	};
	
	std::vector<bool> is_down;
	
protected:
	virtual void update_down( void ) = 0;
	
	static bool drop_input_enabled;
	static int drop_input( const SDL_Event *event );
	
	void push_function_event( Functions ) const;
	
private:
	static const SDLKey push_as_key[functions_count];
	
	static const Uint32 repeat_delay = SDL_DEFAULT_REPEAT_DELAY,
	                    repeat_interval = SDL_DEFAULT_REPEAT_INTERVAL;
	std::vector<Uint32> repeat_tick;
};

class Keyboard : public Controller
{
public:
	Keyboard( void );
	
	bool is_keyboard( void ) { return true; }
	
private:
	void update_down( void );
	
	Uint8 *key_state;
};

class Joystick : public Controller
{
public:
	Joystick( int i = 0 );
	~Joystick( void );
	
	bool is_joystick( void ) { return true; }
	
private:
	void update_down( void );
	
	SDL_Joystick *joystick;
};

extern std::vector<Controller *> controllers, disabled_controllers;

#endif // CONTROLLER_HPP
