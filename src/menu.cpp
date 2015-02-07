#include "controller/controller.hpp"
#include "fixed.hpp"
#include "menu.hpp"
#include "sdl_ext.hpp"
#include "video/font.hpp"
#include "volume.hpp"

SimpleMenu::SimpleMenu( SDL_Surface *background )
: selection(0), no_selection(false),
  background(NULL)
{
	if (background != NULL)
		this->background = SDL_DuplicateRGBSurface(background);
}

SimpleMenu::~SimpleMenu( void )
{
	if (background != NULL)
		SDL_FreeSurface(background);
}

void SimpleMenu::handle_event( SDL_Event &e )
{
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case Controller::BACK_KEY:
		case Controller::QUIT_KEY:
			no_selection = true;
		case Controller::SELECT_KEY:
		case Controller::START_KEY:
			loop_quit = true;
			break;
			
		case Controller::LEFT_SHOULDER_KEY:
		case Controller::LEFT_KEY:
		case Controller::UP_KEY:
			if (selection == 0)
				selection = entry_count();
			--selection;
			break;
			
		case Controller::RIGHT_SHOULDER_KEY:
		case Controller::RIGHT_KEY:
		case Controller::DOWN_KEY:
			++selection;
			if (selection == entry_count())
				selection = 0;
			break;
			
		default:
			break;
		}
		break;
		
	default:
		break;
	}
}

void SimpleMenu::update( void )
{
	// update controller
	for (std::shared_ptr<Controller> &controller : controllers)
		controller->update();
}

void SimpleMenu::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	if (background != NULL)
	{
		SDL_SetAlpha(background, SDL_SRCALPHA, SDL_ALPHA_OPAQUE - alpha / 2);
		SDL_BlitSurface(background, NULL, surface, NULL);
	}
	
	const uint x = surface->w / 2;
	const uint delta_y = font.height(font_sprites[3]) + font.height(font_sprites[3]) / 4;
	uint y = (surface->h - delta_y * entry_count()) / 2;
	
	for (uint i = 0; i < entry_count(); ++i)
	{
		font.blit(surface, x, y, entries[i], font_sprites[3], Font::MAJUSCULE, Font::CENTER, (i == selection) ? alpha : alpha / 2);
		y += delta_y;
	}
}

uint SimpleMenu::entry_count( void ) const
{
	return entries.size();
}

SmoothMenu::SmoothMenu( void )
: selection(0), no_selection(false),
  y(0), y_vel(0), y_accel(Fixed(8) / static_cast<int>(ms_to_updates(500))),
  entry_height(font.height(font_sprites[3]))
{
	// nothing to do
}

void SmoothMenu::handle_event( SDL_Event &e )
{
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case Controller::BACK_KEY:
		case Controller::QUIT_KEY:
			no_selection = true;
			loop_quit = true;
			break;
			
		case Controller::UP_KEY:
		case Controller::LEFT_KEY:
		case Controller::LEFT_SHOULDER_KEY:
			if (selection == 0)
				selection = entry_count();
			--selection;
			break;
			
		case Controller::DOWN_KEY:
		case Controller::RIGHT_KEY:
		case Controller::RIGHT_SHOULDER_KEY:
			if (++selection >= entry_count())
				selection = 0;
			break;
			
		case Controller::SELECT_KEY:
		case Controller::START_KEY:
			no_selection = (entry_count() == 0);
			loop_quit = true;
			break;
			
		default:
			break;
		}
		break;
	}
}

void SmoothMenu::update( void )
{
	// update controller
	for (std::shared_ptr<Controller> &controller : controllers)
		controller->update();
	
	// smooth menu movement
	const int target_y = -entry_height * selection;
	const Fixed y_off = target_y - y;
	
	if (fabsf(y_off) < y_accel)
	{
		y == target_y;
	}
	else
	{
		if ((y_off > 0 && y_accel < 0) || (y_off < 0 && y_accel > 0))
			y_accel = -y_accel;
		
		const Fixed ticks_to_decelerate = y_vel / y_accel,
		            ticks_until_target = (y_vel == 0) ? (y_off / y_accel) : (y_off / y_vel);
		
		if (ticks_to_decelerate < ticks_until_target || ticks_until_target < 0)
			y_vel += y_accel;
		else if (ticks_to_decelerate > ticks_until_target)
			y_vel -= y_accel;
		
		y += y_vel;
	}
}

void SmoothMenu::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	int x = surface->w / 2,
	    y = static_cast<int>(this->y) + (surface->h - font.height(font_sprites[4]) - font.height(font_sprites[4]) / 3) / 2;
	
	if (entry_count() == 0)
		font.blit(surface, x, surface->h / 2, "(EMPTY)", font_sprites[3], Font::CENTER, alpha / 2);
	
	for (uint i = 0; i < entry_count(); ++i)
	{
		if (i == selection)
		{
			y += font.height(font_sprites[4]) / 3;
			font.blit(surface, x, y, entries[i], font_sprites[4], Font::CENTER, alpha);
			y += font.height(font_sprites[4]);
			y += font.height(font_sprites[4]) / 3;
		}
		else
		{
			if (y > surface->h)
				break;
			else if (y > -static_cast<int>(font.height(font_sprites[3])))
				font.blit(surface, x, y, entries[i], font_sprites[3], Font::CENTER, alpha / 2);
			y += font.height(font_sprites[3]);
		}
	}
}

uint SmoothMenu::entry_count( void ) const
{
	return entries.size();
}

