#include "file_system.hpp"

using namespace std;

vector<string> directory_listing( const string &directory )
{
	vector<string> entries;
	
	DIR *dir = opendir(directory.c_str());
	if (dir != NULL)
	{
		struct dirent *dir_ent;
		while ((dir_ent = readdir(dir)) != NULL)
		{
			entries.push_back(dir_ent->d_name);
		}
		
		closedir(dir);
	}
	
	return entries;
}

bool path_exists( const string &path )
{
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}
