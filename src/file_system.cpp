#include "file_system.hpp"

std::vector<std::string> read_directory_listing( const std::string &directory )
{
	std::vector<std::string> entries;
	
	DIR *dir = opendir(directory.c_str());
	if (dir != NULL)
	{
		struct dirent *dir_ent;
		while ((dir_ent = readdir(dir)) != NULL)
		{
			const std::string entry = dir_ent->d_name;
			
			if (entry != "." && entry != "..")
				entries.push_back(entry);
		}
		
		closedir(dir);
	}
	
	return entries;
}

bool path_exists( const std::string &path )
{
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}
