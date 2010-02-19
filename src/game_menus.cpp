#include "ball.hpp"
#include "controller/controller.hpp"
#include "editor.hpp"
#include "file_system.hpp"
#include "game_menus.hpp"
#include "highscore.hpp"
#include "main.hpp"
#include "video/video.hpp"
#include "video/font.hpp"

using namespace std;

GameMenu::GameMenu( void )
: ball(screen_width / 2, screen_height)
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

void GameMenu::update( void )
{
	// update controller
	BOOST_FOREACH (boost::shared_ptr<Controller> &controller, controllers)
		controller->update();
	
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
	
	font.blit(surface, 0, surface->h - font.height(font_sprites[1]), "v0.3 BETA", font_sprites[1], Font::majuscule, Font::left, alpha);
	
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

LevelSetMenu::LevelSetMenu( bool allow_new )
{
	vector<string> dir_entries = directory_listing(level_directory);
	
	// populate the level set list
	BOOST_FOREACH (const string &dir_entry, dir_entries)
	{
		LevelSet entry(level_directory + dir_entry);
		if (!entry.invalid())
			entries.push_back(entry);
	}
	
	sort(entries.begin(), entries.end());
	
	if (allow_new)
	{
		LevelSet new_level_set;
		new_level_set.name = "New Level Set...";
		boost::to_upper(new_level_set.name);
		entries.push_back(new_level_set);
	}
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

uint LevelSetMenu::entry_count( void ) const
{
	return entries.size();
}

LevelMenu::LevelMenu( const LevelSet &level_set, bool allow_new )
{
	BOOST_FOREACH (const Level &level, level_set.levels)
		entries.push_back(level.get_name());
	
	if (allow_new)
	{
		static string new_level = "New Level...";
		boost::to_upper(new_level);
		entries.push_back(new_level);
	}
}

ScoredLevelMenu::ScoredLevelMenu( const LevelSet &level_set, bool show_one_incomplete, bool auto_select_single_entry )
: auto_select_single_entry(auto_select_single_entry)
{
	BOOST_FOREACH (const Level &level, level_set.levels)
	{
		if (path_exists(level.get_score_path()))
		{
			Entry entry = { level.get_name(), level.get_path(), Highscore(level.get_score_path()).time() };
			entries.push_back(entry);
		}
		else if (show_one_incomplete)
		{
			Entry entry = { level.get_name(), level.get_path(), "UNCOMPLETED" };
			entries.push_back(entry);
			break;
		}
		else
			break;
	}
}

void ScoredLevelMenu::loop( SDL_Surface *surface )
{
	if (!(auto_select_single_entry && entry_count() == 1))
		SmoothMenu::loop(surface);
}

void ScoredLevelMenu::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	if (entry_count() == 0)
		font.blit(surface, surface->w / 2, surface->h / 2, "(EMPTY)", font_sprites[2], Font::center, alpha / 2);
	
	int y = static_cast<int>(this->y) + (surface->h - font.height(font_sprites[3]) - font.height(font_sprites[3]) / 2) / 2;
	
	for (uint i = 0; i < entries.size(); ++i)
	{
		if (i == selection)
		{
			y += font.height(font_sprites[4]) / 3;
			font.blit(surface, surface->w / 2, y, entries[i].name, font_sprites[4], Font::center, alpha);
			y += font.height(font_sprites[4]);
			font.blit(surface, surface->w - 1, y, entries[i].time, font_sprites[1], Font::right, alpha);
			y += font.height(font_sprites[4]) / 3;
		}
		else
		{
			if (y > surface->h)
				break;
			else if (y > -static_cast<int>(font.height(font_sprites[3])))
			{
				font.blit(surface, surface->w - 1, y + font.height(font_sprites[3]) - font.height(font_sprites[1]), entries[i].time, font_sprites[1], Font::right, alpha);
				font.blit(surface, surface->w / 2, y, entries[i].name, font_sprites[3], Font::center, alpha / 2);
			}
			y += font.height(font_sprites[3]);
		}
	}
}

uint ScoredLevelMenu::entry_count( void ) const
{
	return entries.size();
}
