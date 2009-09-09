#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "SDL.h"

class Controller
{
public:
	Controller( void );
	virtual ~Controller( void ) { } // work-around a gcc 4.1.1 warning
	
	void update( void );
	
	virtual bool is_keyboard( void ) { return false; }
	virtual bool is_fake( void ) { return false; }
	
	static const int functions_count = 10;
	enum functions
	{
		up = 0,
		right,
		down,
		left,
		
		select,
		back,
		
		start,
		quit,
		
		vol_up,
		vol_down
	};
	
	std::vector<bool> is_down;
	
protected:
	virtual void do_update( void ) = 0;
	
	static bool drop_input_enabled;
	static int drop_input( const SDL_Event *event );
	
	void push_function_event( functions ) const;
	
private:
	static const int down_repeat_ticks = 15;
	std::vector<int> down_ticks;
};

class Keyboard : public Controller
{
public:
	Keyboard( void );
	
	bool is_keyboard( void ) { return true; }
	
private:
	void do_update( void );
	
	Uint8 *key_state;
};

class Joystick : public Controller
{
public:
	Joystick( int i = 0 );
	~Joystick( void );
	
	bool is_joystick( void ) { return true; }
	
private:
	void do_update( void );
	
	SDL_Joystick *joystick;
};

extern std::vector<Controller *> controllers, disabled_controllers;

#endif // CONTROLLER_HPP
