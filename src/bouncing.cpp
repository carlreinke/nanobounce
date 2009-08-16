#include "audio.h"
#include "bouncing.h"
#include "controller.h"
#include "menu.h"
#include "video.h"

using namespace std;

Uint32 push_frame_event( Uint32, void * );
Uint32 push_update_event( Uint32, void * );

int fps = 50, ups = 40;
int frame_ticks = 1000 / min(fps, ups),
    update_ticks = 1000 / ups;

//! in-game menu
//! best-time high score list
//! add particle class

map<int, Sprite> font_sprites;

bool global_quit = false;

int main( int argc, char *argv[] )
{
	if (SDL_Init(SDL_INIT_TIMER) == -1)
	{
		cerr << SDL_GetError() << endl;
		exit(EXIT_FAILURE);
	}
	SDL_Surface *surface = init_video();
	init_audio();
	
	controllers.push_back(new Keyboard());
	controllers.push_back(new Joystick(0));
	
	{
		SDL_Color color = { 100, 100, 100 };
		font_sprites[1] = Sprite(1, 1, color);
		font_sprites[2] = Sprite("img/font_gray_2.ppm");
		font_sprites[3] = Sprite("img/font_gray_3.ppm");
		font_sprites[4] = Sprite("img/font_gray_4.ppm");
	}
	
	SDL_TimerID frame_timer = SDL_AddTimer(0, push_frame_event, NULL);
	SDL_TimerID update_timer = SDL_AddTimer(0, push_update_event, NULL);
	
	game_menu(surface);
	
	SDL_RemoveTimer(frame_timer);
	SDL_RemoveTimer(update_timer);
	
	for (vector<Controller *>::iterator i = controllers.begin(); i != controllers.end(); )
	{
		delete *i;
		controllers.erase(i);
	}
	
	deinit_audio();
	SDL_Quit();
	
	return EXIT_SUCCESS;
}

Uint32 push_frame_event( Uint32 interval, void * )
{
	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user.type = SDL_USEREVENT;
	event.user.code = USER_FRAME;
	
	SDL_PushEvent(&event);
	
	return frame_ticks;
}

Uint32 push_update_event( Uint32 interval, void * )
{
	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user.type = SDL_USEREVENT;
	event.user.code = USER_UPDATE;
	
	SDL_PushEvent(&event);
	
	return update_ticks;
}
