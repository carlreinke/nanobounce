#include "ball.hpp"
#include "controller.hpp"
#include "editor.hpp"
#include "file_system.hpp"
#include "font.hpp"
#include "highscore.hpp"
#include "main.hpp"
#include "menu.hpp"
#include "misc.hpp"
#include "sdl_ext.hpp"
#include "volume.hpp"

using namespace std;

SimpleMenu::SimpleMenu( SDL_Surface *background )
: selection(0), no_selection(false)
{
	this->background = SDL_DuplicateRGBSurface(background);
}

SimpleMenu::~SimpleMenu( void )
{
	SDL_FreeSurface(background);
}

void SimpleMenu::handle_event( SDL_Event &e )
{
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case Controller::back_key:
		case Controller::quit_key:
			no_selection = true;
		case Controller::select_key:
		case Controller::start_key:
			loop_quit = true;
			break;
			
		case Controller::left_shoulder_key:
		case Controller::left_key:
		case Controller::up_key:
			if (selection == 0)
				selection = entries.size();
			--selection;
			break;
			
		case Controller::right_shoulder_key:
		case Controller::right_key:
		case Controller::down_key:
			++selection;
			if (selection == entries.size())
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
	for (vector< boost::shared_ptr<Controller> >::iterator c = controllers.begin(); c != controllers.end(); ++c)
		(*c)->update();
}

void SimpleMenu::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	SDL_SetAlpha(background, SDL_SRCALPHA, SDL_ALPHA_OPAQUE - alpha / 2);
	SDL_BlitSurface(background, NULL, surface, NULL);
	
	const uint x = surface->w / 2;
	const uint delta_y = font.height(font_sprites[3]) + font.height(font_sprites[3]) / 4;
	uint y = (surface->h - delta_y * entries.size()) / 2;
	
	for (uint i = 0; i < entries.size(); ++i)
	{
		font.blit(surface, x, y, entries[i], font_sprites[3], Font::majuscule, Font::center, (i == selection) ? alpha : alpha / 2);
		y += delta_y;
	}
}

SmoothMenu::SmoothMenu( void )
: selection(0), no_selection(false),
  y(0), y_vel(0), y_accel(Fixed(8) / static_cast<int>(ms_to_updates(250)))
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
		case Controller::back_key:
		case Controller::quit_key:
			no_selection = true;
			loop_quit = true;
			break;
			
		case Controller::up_key:
		case Controller::left_key:
		case Controller::left_shoulder_key:
			if (selection == 0)
				selection = entries.size();
			--selection;
			break;
			
		case Controller::down_key:
		case Controller::right_key:
		case Controller::right_shoulder_key:
			if (++selection >= entries.size())
				selection = 0;
			break;
			
		case Controller::select_key:
		case Controller::start_key:
			no_selection = entries.empty();
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
	for (vector< boost::shared_ptr<Controller> >::iterator c = controllers.begin(); c != controllers.end(); ++c)
		(*c)->update();
	
	// smooth menu movement
	const int target_y = -font.height(font_sprites[3]) * selection;
	const Fixed y_off = target_y - y;
	
	if (fabsf(y_off) < 1)
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
	
	int y = static_cast<int>(this->y) + (surface->h - font.height(font_sprites[3]) - font.height(font_sprites[3]) / 2) / 2;
	
	if (entries.size() == 0)
		font.blit(surface, surface->w / 2, surface->h / 2, "(EMPTY)", font_sprites[2], Font::center, alpha / 2);
	
	for (uint i = 0; i < entries.size(); ++i)
	{
		if (i == selection)
		{
			y += font.height(font_sprites[4]) / 3;
			font.blit(surface, surface->w / 2, y, *entries[i], font_sprites[4], Font::center, alpha);
			y += font.height(font_sprites[4]);
			y += font.height(font_sprites[4]) / 3;
		}
		else
		{
			if (y > surface->h)
				break;
			else if (y > -static_cast<int>(font.height(font_sprites[3])))
				font.blit(surface, surface->w / 2, y, *entries[i], font_sprites[3], Font::center, alpha / 2);
			y += font.height(font_sprites[3]);
		}
	}
}

GameMenu::GameMenu( void )
: selection(0), ball(screen_width / 2, screen_height)
{
	const string menu_items[] =
	{
		"Play",
		"Edit",
		"Quit"
	};
	for (uint i = 0; i < COUNTOF(menu_items); ++i)
		entries.push_back(menu_items[i]);
}

void GameMenu::handle_event( SDL_Event &e )
{
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case Controller::back_key:
		case Controller::quit_key:
			loop_quit = true;
			break;
			
		case Controller::left_key:
		case Controller::left_shoulder_key:
			if (selection > 0)
				--selection;
			break;
			
		case Controller::right_key:
		case Controller::right_shoulder_key:
			if (selection < entries.size() - 1)
				++selection;
			break;
			
		case Controller::select_key:
		case Controller::start_key:
			if (selection == 2)
			{
				loop_quit = true;
			}
			else
			{
				// at this point, ticks should be a decent seed
				srand(SDL_GetTicks());
				
				LevelSetMenu set_menu;
				set_menu.loop(surface);
				
				if (!set_menu.no_selection)
				{
					LevelSet &level_set = set_menu.entries[set_menu.selection];
					
					switch (selection)
					{
					case 0:
						level_set.play(surface);
						break;
						
					case 1:
						level_set.load_levels();
						
						LevelMenu level_menu(level_set);
						level_menu.loop(surface);
						
						if (!level_menu.no_selection)
						{
							Level &level = level_set.levels[level_menu.selection];
							
#ifndef TARGET_GP2X
							SDL_ShowCursor(SDL_ENABLE);
#endif
							
							Editor editor;
							editor.load(level.path);
							editor.loop(surface);
							
#ifndef TARGET_GP2X
							SDL_ShowCursor(SDL_DISABLE);
#endif
						}
						break;
					}
				}
			}
			break;
			
		default:
			break;
		}
		break;
	}
}

void GameMenu::update( void )
{
	// update controller
	for (vector< boost::shared_ptr<Controller> >::iterator c = controllers.begin(); c != controllers.end(); ++c)
		(*c)->update();
	
	for (uint i = 0; i < ups_multiplier; ++i)
	{
		int x = screen_width * (selection + 1) / (entries.size() + 1),
			y = screen_height / 2 + font.height(font_sprites[4]) * 3 / 2 + font.height(font_sprites[3]);
		
		ball.tick(ball.x > x ? -1 : 1);
		if (ball.y > y)
			ball.y_vel = -ball.y_term_vel;
	}
}

void GameMenu::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	ball.draw(surface, 0, 0, alpha);
	
	// TODO: remove for non-beta
	font.blit(surface, ball.x, screen_height - font.height(font_sprites[3]) * 2, "Private beta release!", font_sprites[1], Font::majuscule, Font::center, alpha);
	
	int x = surface->w / 2,
	    y = surface->h / 2 - font.height(font_sprites[4]) * 3 / 2;
	
	font.blit(surface, x, y, "Nanobounce", font_sprites[4], Font::majuscule, Font::center, alpha);
	y += font.height(font_sprites[4]) * 3;
	
	for (uint i = 0; i < entries.size(); ++i)
	{
		x = surface->w * (i + 1) / (entries.size() + 1);
		font.blit(surface, x, y, entries[i], font_sprites[3], Font::majuscule, Font::center, (i == selection) ? alpha : alpha / 2);
	}
	
	font.blit(surface, 0, surface->h - font.height(font_sprites[1]), "v0.2 BETA", font_sprites[1], Font::majuscule, Font::left, alpha);
	
	x = surface->w - 1;
	y = surface->h - font.height(font_sprites[1]) * 3;
	
	font.blit(surface, x, y, "programming, sprites, and effects:", font_sprites[1], Font::majuscule, Font::right, alpha);
	y += font.height(font_sprites[1]);
	font.blit(surface, x, y, "Carl \"Mindless\" Reinke", font_sprites[1], Font::majuscule, Font::right, alpha);
	y += font.height(font_sprites[1]);
	font.blit(surface, x, y, "music: Jakob Svanholm, Seth Peelle", font_sprites[1], Font::majuscule, Font::right, alpha);
	
#ifdef TARGET_GP2X
	x = surface->w / 2;
	y = surface->h / 2;
	
	font.blit(surface, x, y, "Tip: This game is least frustrating", font_sprites[1], Font::majuscule, Font::center, alpha);
	y += font.height(font_sprites[1]);
	font.blit(surface, x, y, "when played using the shoulder buttons.", font_sprites[1], Font::majuscule, Font::center, alpha);
	y += font.height(font_sprites[1]);
	font.blit(surface, x, y, "Enjoy!", font_sprites[1], Font::majuscule, Font::center, alpha);
#endif
}

LevelSetMenu::LevelSetMenu( void )
{
	vector<string> dir_entries = directory_listing(level_directory);
	
	// populate the level set list
	for (vector<string>::const_iterator dir_entry = dir_entries.begin(); dir_entry != dir_entries.end(); ++dir_entry)
	{
		LevelSet entry(level_directory + *dir_entry);
		if (!entry.invalid())
			entries.push_back(entry);
	}
	
	sort(entries.begin(), entries.end());
	
	SmoothMenu::entries.resize(entries.size());
}

void LevelSetMenu::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	int y = static_cast<int>(this->y) + (surface->h - font.height(font_sprites[3]) - font.height(font_sprites[3]) / 2) / 2;
	
	for (uint i = 0; i < entries.size(); ++i)
	{
		if (i == selection)
		{
			y += font.height(font_sprites[4]) / 3;
			font.blit(surface, surface->w / 2, y, entries[i].name, font_sprites[4], Font::center, alpha);
			y += font.height(font_sprites[4]);
			font.blit(surface, surface->w / 2, y, entries[i].author, font_sprites[1], Font::center, alpha);
			y += font.height(font_sprites[1]);
			y += font.height(font_sprites[4]) / 3;
		}
		else
		{
			if (y > surface->h)
				break;
			else if (y > -static_cast<int>(font.height(font_sprites[3])))
				font.blit(surface, surface->w / 2, y, entries[i].name, font_sprites[3], Font::center, alpha / 2);
			y += font.height(font_sprites[3]);
		}
	}
}

LevelMenu::LevelMenu( const LevelSet &level_set )
{
	for (vector<Level>::const_iterator level = level_set.levels.begin(); level != level_set.levels.end(); ++level)
		entries.push_back(&level->name);
}
