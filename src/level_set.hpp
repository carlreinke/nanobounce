#ifndef LEVEL_SET_HPP
#define LEVEL_SET_HPP

#include "level.hpp"

class LevelSet
{
public:
	LevelSet( void );
	LevelSet( const std::string &directory );
	bool invalid( void ) const { return !valid; }
	
	bool operator<( const LevelSet &that ) const { return this->name < that.name; }
	
	void load_levels( void );
	void save_meta( void );
	void append_level( Level &level );
	
private:
	bool valid;
	std::string directory;
	
	std::string name, author;
	std::vector<Level> levels;
	
	friend int main(int, char **);
	friend class Editor;
	friend class LevelMenu;
	friend class ScoredLevelMenu;
	friend class LevelSetCongratsLoop;
	friend class LevelSetMenu;
};

#endif // LEVEL_SET_HPP
