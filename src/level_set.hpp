#ifndef LEVEL_SET_HPP
#define LEVEL_SET_HPP

#include "level.hpp"
#include "loop.hpp"
#include "particle.hpp"

class LevelSet
{
public:
	LevelSet( const std::string &directory );
	bool invalid( void ) const { return !valid; }
	bool operator<( const LevelSet &that ) const { return this->name < that.name; }
	
	void load_levels( void );
	void play( SDL_Surface * );
	
private:
	bool valid;
	std::string directory;
	
	std::string name, author;
	std::vector<Level> levels;
	
	class CongratsLoop : public Loop
	{
	public:
		CongratsLoop( const LevelSet & );
		
		void handle_event( SDL_Event & );
		void update( void );
		void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
		
	private:
		const std::string set_name;
		
		std::list<Particle> particles;
	};
	
	friend class GameMenu;
	friend class LevelMenu;
	friend class LevelSetMenu;
};

class LevelIntroLoop : public Loop
{
public:
	LevelIntroLoop( const Level &, const Highscore & );
	
	void handle_event( SDL_Event & );
	void update( void );
	void draw( SDL_Surface *, Uint8 alpha = SDL_ALPHA_OPAQUE ) const;
	
private:
	const std::string level_name;
	const Highscore score;
	
	int ticks;
};

#endif // LEVEL_SET_HPP
