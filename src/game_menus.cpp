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
		"More",
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
	
	int x = screen_width * (selection + 1) / (entry_count() + 1),
	    y = screen_height / 2 + font.height(font_sprites[4]) * 3 / 2 + font.height(font_sprites[3]);
	
	ball.tick(ball.x > x ? -1 : 1);
	if (ball.y > y)
		ball.y_vel = -ball.y_term_vel;
}

void GameMenu::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	ball.draw(surface, 0, 0, alpha);
	
	int x = surface->w / 2,
	    y = surface->h / 2 - font.height(font_sprites[4]) * 3 / 2;
	
	font.blit(surface, x, y, "Nanobounce", font_sprites[4], Font::majuscule, Font::center, alpha);
	y += font.height(font_sprites[4]) * 3;
	
	for (uint i = 0; i < entry_count(); ++i)
	{
		x = surface->w * (i + 1) / (entry_count() + 1);
		font.blit(surface, x, y, entries[i], font_sprites[3], Font::majuscule, Font::center, (i == selection) ? alpha : alpha / 2);
	}
	
	font.blit(surface, 0, surface->h - font.height(font_sprites[1]), "v0.3e", font_sprites[1], Font::majuscule, Font::left, alpha);
	
	x = surface->w - 1;
	y = surface->h - font.height(font_sprites[1]) * 3;
	
	font.blit(surface, x, y, "programming, sprites, and effects:", font_sprites[1], Font::majuscule, Font::right, alpha);
	y += font.height(font_sprites[1]);
	font.blit(surface, x, y, "Carl \"Mindless\" Reinke", font_sprites[1], Font::majuscule, Font::right, alpha);
	y += font.height(font_sprites[1]);
	font.blit(surface, x, y, "music: Jakob Svanholm, Seth Peelle", font_sprites[1], Font::majuscule, Font::right, alpha);
	
#ifdef HAS_SHOULDER_BUTTONS
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
	
	int x = surface->w / 2,
	    y = static_cast<int>(this->y) + (surface->h - font.height(font_sprites[4]) - font.height(font_sprites[4]) / 3) / 2;
	
	for (uint i = 0; i < entry_count(); ++i)
	{
		if (i == selection)
		{
			y += font.height(font_sprites[4]) / 3;
			font.blit(surface, x, y, entries[i].name, font_sprites[4], Font::center, alpha);
			y += font.height(font_sprites[4]);
			font.blit(surface, x, y, entries[i].author, font_sprites[1], Font::center, alpha);
			y += font.height(font_sprites[1]);
			y += font.height(font_sprites[4]) / 3;
		}
		else
		{
			if (y > surface->h)
				break;
			else if (y > -static_cast<int>(font.height(font_sprites[3])))
				font.blit(surface, x, y, entries[i].name, font_sprites[3], Font::center, alpha / 2);
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
			Highscore score(level.get_score_path());
			Entry entry = { level.get_name(), level.get_path(), score.name, score.time() };
			entries.push_back(entry);
		}
		else if (show_one_incomplete)
		{
			Entry entry = { level.get_name(), level.get_path(), "", "UNCOMPLETED" };
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
	
	int x = surface->w / 2, x_right = surface->w - 1,
	    y = static_cast<int>(this->y) + (surface->h - font.height(font_sprites[4]) - font.height(font_sprites[4]) / 3) / 2;
	
	if (entry_count() == 0)
		font.blit(surface, x, surface->h / 2, "(EMPTY)", font_sprites[3], Font::center, alpha / 2);
	
	for (uint i = 0; i < entry_count(); ++i)
	{
		const Entry &entry = entries[i];
		
		if (i == selection)
		{
			string subtext = (!entry.score_name.empty() ? entry.score_name + ": " : "") + entry.score_time;
			
			y += font.height(font_sprites[4]) / 3;
			font.blit(surface, x, y, entry.name, font_sprites[4], Font::center, alpha);
			y += font.height(font_sprites[4]);
			font.blit(surface, x_right, y, subtext, font_sprites[1], Font::right, alpha);
			y += font.height(font_sprites[4]) / 3;
		}
		else
		{
			if (y > surface->h)
				break;
			else if (y > -static_cast<int>(font.height(font_sprites[3])))
			{
				font.blit(surface, x_right, y + font.height(font_sprites[3]) - font.height(font_sprites[1]), entry.score_time, font_sprites[1], Font::right, alpha);
				font.blit(surface, x, y, entry.name, font_sprites[3], Font::center, alpha / 2);
			}
			y += font.height(font_sprites[3]);
		}
	}
}

uint ScoredLevelMenu::entry_count( void ) const
{
	return entries.size();
}

TextEntryMenu::TextEntryMenu( const string &title, const string &text )
: text(text), title(title)
{
	entries.push_back("END");
	
	typedef std::pair<int, int> Range;
	std::pair<int, int> ranges[] =
	{
		make_pair('A', 'Z'),
		make_pair('a', 'z'),
		make_pair('0', '9'),
		
		make_pair(      0, '0' - 1),
		make_pair('9' + 1, 'A' - 1),
		make_pair('Z' + 1, 'a' - 1),
		make_pair('z' + 1, 255),
	};
	BOOST_FOREACH (const Range &range, ranges)
	{
		for (int c = range.first; c <= range.second; ++c)
		{
			const string character = boost::lexical_cast<string>(static_cast<char>(c));
			if (font.width(character, font_sprites[1]) > 0)
				entries.push_back(character);
		}
	}
	
	entry_height = font.height(font_sprites[1]);
	y_accel = static_cast<int>(entry_height) / 2;
}

void TextEntryMenu::handle_event( SDL_Event &e )
{
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case Controller::back_key:
			if (text.size() == 0)
				goto made_no_selection;
			text.resize(text.size() - 1);
			break;
			
		case Controller::select_key:
			if (entries[selection].size() > 1) // selected "end"
				goto made_selection;
			text += entries[selection];
			break;
			
		case Controller::quit_key:
made_no_selection:
			no_selection = true;
		case Controller::start_key:
made_selection:
			loop_quit = true;
			break;
			
		case Controller::left_shoulder_key:
		case Controller::left_key:
		case Controller::up_key:
			if (selection == 0)
				selection = entry_count();
			--selection;
			break;
			
		case Controller::right_shoulder_key:
		case Controller::right_key:
		case Controller::down_key:
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

void TextEntryMenu::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	int x = surface->w / 2,
	    y = (surface->h - font.height(font_sprites[4])) / 2;
	
	font.blit(surface, x, y, text, font_sprites[4], Font::center, alpha);
	
	x += font.width(text, font_sprites[4]) / 2;
	y += static_cast<int>(this->y);
	
	for (uint i = 0; i < entry_count(); ++i)
	{
		if (i == selection)
		{
			if (entries[i].length() > 1) // "end"
				font.blit(surface, x, y + font.height(font_sprites[4]) / 2, entries[i], font_sprites[1], Font::left, alpha);
			else
				font.blit(surface, x, y, entries[i], font_sprites[4], Font::left, alpha);
			y += font.height(font_sprites[4]);
		}
		else
		{
			if (y > surface->h)
				break;
			else if (y > -static_cast<int>(font.height(font_sprites[3])))
				font.blit(surface, x, y, entries[i], font_sprites[1], Font::left, alpha / 2);
			y += entry_height;
		}
	}
	
	x = surface->w / 2;
	y = (surface->h - font.height(font_sprites[3])) / 4;
	
	font.blit(surface, x, y, title, font_sprites[3], Font::majuscule, Font::center, alpha);
}
