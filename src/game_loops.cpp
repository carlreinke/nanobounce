#include "audio/audio.hpp"
#include "file_system.hpp"
#include "game.hpp"
#include "game_loops.hpp"
#include "main.hpp"
#include "video/font.hpp"

LevelPackCongratsLoop::LevelPackCongratsLoop( const LevelPack &level_pack )
: pack_name(level_pack.get_name())
{
	play_music(music_directory + "special/Vulpine Skyflight.ogg");
}

void LevelPackCongratsLoop::handle_event( SDL_Event &e )
{
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case Controller::BACK_KEY:
		case Controller::QUIT_KEY:
		case Controller::SELECT_KEY:
		case Controller::START_KEY:
			loop_quit = true;
			break;
			
		default:
			break;
		}
		
	default:
		break;
	}
}

void LevelPackCongratsLoop::update( void )
{
	for (std::shared_ptr<Controller> controller : controllers)
		controller->update();
	
	// TODO: fix: fireworks require old tick value, so they only tick every other time
	static int i = 0;
	if (++i % 4 != 0)
		return;
	
	// firework colors
	static const SDL_Color red = SDL_Color_RGBA(224, 32, 32),
	                       yellow = SDL_Color_RGBA(224, 160, 32),
	                       green = SDL_Color_RGBA(32, 224, 64),
	                       blue = SDL_Color_RGBA(64, 32, 224),
	                       purple = SDL_Color_RGBA(224, 32, 192),
	                       white = SDL_Color_RGBA(255, 255, 255);
	static const SDL_Color *preset_colors[][3] =
	{
		// solids
		{ &red, &red, &red },
		{ &green, &green, &green },
		{ &blue, &blue, &blue },
		{ &white, &white, &white },
		
		// almost solids
		{ &red, &red, &yellow },
		{ &blue, &blue, &purple },
		
		// white highlights
		{ &red, &red, &white },
		{ &green, &green, &white },
		{ &blue, &blue, &white },
		{ &purple, &purple, &white },
		
		{ &red, &green, &blue },
		{ &red, &white, &blue },  // yay, US
	};
	
	// random fireworks
	if (rand() % 20 == 0)
	{
		const int x = rand() % (screen_width - screen_width / 4) + (screen_width / 8),
		          y = rand() % (screen_height - screen_height / 3) + (screen_height / 8);
		
		const SDL_Color **colors = preset_colors[(x + y) % COUNTOF(preset_colors)];
		
		for (uint i = 0; i < 40; ++i)
			particles.push_back(FireworkParticle(x, y, *colors[i % COUNTOF(*preset_colors)]));
		
		play_sample(Game::samples.nuke, make_frac<Fixed>(1, 5), Fixed(x) / screen_width);
	}
	
	Particle::tick_all(particles);
}

void LevelPackCongratsLoop::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	for (const Particle &particle : particles)
		particle.draw(surface, 0, 0, alpha);
	
	font.blit(surface, surface->w / 2, surface->h / 4, "Congratulations!", font_sprites[3], Font::MAJUSCULE, Font::CENTER, alpha);
	font.blit(surface, surface->w / 2, surface->h / 2, pack_name, font_sprites[4], Font::CENTER, alpha);
	font.blit(surface, surface->w / 2, surface->h / 2 + font.height(font_sprites[4]), "completed!", font_sprites[3], Font::MAJUSCULE, Font::CENTER, alpha);
}

FireworkParticle::FireworkParticle( Fixed x, Fixed y, const SDL_Color &color )
: Particle(x, y, rand() % 20 + 30, color)
{
	const Fixed radius = make_frac<Fixed>(rand() % (1024 * 2) - 1024, 1024),  // [-1..1]
	            angle = make_frac<Fixed>(rand() % 31415, 10000);  // [0..pi]
	
	x_vel = cosf(angle) * radius,
	y_vel = sinf(angle) * radius;
	
	y_accel /= 2;
	
	term_vel = 100;  // large enough to be irrelevant
	
	alpha = SDL_ALPHA_OPAQUE + static_cast<int>(ticks_to_live) - 20 - 30;
	alpha_per_tick = -alpha / static_cast<int>(ticks_to_live);
}

LevelIntroLoop::LevelIntroLoop( const Level &level )
: level_name(level.get_name()),
  ticks(ms_to_updates(2000))
{
	// nothing to do
}

void LevelIntroLoop::handle_event( SDL_Event &e )
{
	switch (e.type)
	{
	case SDL_KEYDOWN:
		switch (e.key.keysym.sym)
		{
		case Controller::BACK_KEY:
		case Controller::QUIT_KEY:
		case Controller::SELECT_KEY:
		case Controller::START_KEY:
			loop_quit = true;
			break;
			
		default:
			break;
		}
		
	default:
		break;
	}
}

void LevelIntroLoop::update( void )
{
	for (std::shared_ptr<Controller> controller : controllers)
		controller->update();
	
	if (--ticks == 0)
		loop_quit = true;
}

void LevelIntroLoop::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	int x = surface->w / 2,
	    y = surface->h / 2 - font.height(font_sprites[4]) / 2;
	
	font.blit(surface, x, y, level_name, font_sprites[4], Font::CENTER, alpha);
}

LevelWonLoop::LevelWonLoop( const Level &level, const Highscore &best_score, const Highscore &new_score )
: level_name(level.get_name()), best_score(best_score), new_score(new_score)
{
	const std::string menu_items[] =
	{
		"Next",
		"Retry"
	};
	for (uint i = 0; i < COUNTOF(menu_items); ++i)
		entries.push_back(menu_items[i]);
}

void LevelWonLoop::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	SDL_FillRect(surface, NULL, 0);
	
	const int x = surface->w / 2;
	int y = surface->h * 1 / 5 - font.height(font_sprites[4]) / 2;
	
	font.blit(surface, x, y, level_name, font_sprites[4], Font::CENTER, alpha);
	
	y = surface->h * 2 / 5 - font.height(font_sprites[3]);
	font.blit(surface, x, y, "Best Time:", font_sprites[1], Font::MAJUSCULE, Font::CENTER, alpha);
	y += font.height(font_sprites[1]);
	font.blit(surface, x, y, best_score.get_player_name(), font_sprites[3], Font::CENTER, alpha);
	y += font.height(font_sprites[3]);
	font.blit(surface, x, y, best_score.get_time(), font_sprites[3], Font::CENTER, alpha);
	
	y = surface->h * 3 / 5 - font.height(font_sprites[3]) / 2;
	font.blit(surface, x, y, "Your Time:", font_sprites[1], Font::MAJUSCULE, Font::CENTER, alpha);
	y += font.height(font_sprites[1]);
	font.blit(surface, x, y, new_score.get_time(), font_sprites[3], Font::CENTER, alpha);
	y += font.height(font_sprites[3]);
	font.blit(surface, x, y, "(+" + Highscore::format_time(new_score.get_time_ms() - best_score.get_time_ms()) + ")", font_sprites[1], Font::CENTER, alpha);
	
	y = surface->h * 4 / 5;
	for (uint i = 0; i < entry_count(); ++i)
	{
		int x = surface->w * (i + 1) / (entry_count() + 1);
		font.blit(surface, x, y, entries[i], font_sprites[3], Font::MAJUSCULE, Font::CENTER, (i == selection) ? alpha : alpha / 2);
	}
}

LevelWonBestTimeLoop::LevelWonBestTimeLoop( const Level &level, const Highscore &new_score )
: TextEntryMenu("New Best Time!", new_score.get_player_name()),
  level_name(level.get_name()), new_score(new_score),
  ticks(0)
{
	// nothing to do
}

void LevelWonBestTimeLoop::update( void )
{
	TextEntryMenu::update();
	
	particles.push_back(WooshParticle((rand() % 2 == 1 ? 0 : screen_width), rand() % screen_height, SDL_Color_RGBA(64, 255, 64)));
	
	Particle::tick_all(particles);
}

void LevelWonBestTimeLoop::draw( SDL_Surface *surface, Uint8 alpha ) const
{
	TextEntryMenu::draw(surface, alpha);
	
	for (const Particle &particle : particles)
		particle.draw(surface, 0, 0, alpha);
	
	int x = surface->w / 2,
	    y = surface->h * 3 / 4 - font.height(font_sprites[4]);
	
	font.blit(surface, x, y, level_name, font_sprites[4], Font::CENTER, alpha);
	y += font.height(font_sprites[4]);
	font.blit(surface, x, y, new_score.get_time(), font_sprites[3], Font::CENTER, alpha);
}

WooshParticle::WooshParticle( Fixed x, Fixed y, const SDL_Color &color )
: Particle(x, y, rand() % 100 + 50, color)
{
	x_vel = make_frac<Fixed>(rand() % 1024 + 1024, 1024 * 2) * (x > screen_width / 2 ? -1 : 1);  // [-1..1]
	y_vel = 0;
	
	x_accel = y_accel = 0;
	
	alpha = SDL_ALPHA_OPAQUE + static_cast<int>(ticks_to_live) - 50 - 50;
	alpha_per_tick = -alpha / static_cast<int>(ticks_to_live);
}

