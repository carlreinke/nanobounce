#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "SDL.h"

class Controller
{
public:
	Controller( void );
	
	void update( void );
	
	virtual bool is_keyboard( void ) { return false; }
	virtual bool is_joystick( void ) { return false; }
	
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
	static const int down_repeat_ticks = 25;
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

class Highscore
{
public:
	Highscore( int ms_per_tick );
	Highscore( std::istream & );
	
	void save( std::ostream & );
	std::string time( void );
	
	std::string name;
	std::vector<int> x_push_direction;
	
private:
	int ms_per_tick;
};

class Replay : public Controller
{
public:
	Replay( const Highscore & );
	
private:
	void do_update( void );
	
	Highscore data;
};

extern std::vector<Controller *> controllers, disabled_controllers;

#endif
