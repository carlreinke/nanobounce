#include "file_system.hpp"

static boost::filesystem::path get_user_data_directory();

boost::filesystem::path user_data_directory = get_user_data_directory();

static boost::filesystem::path get_user_data_directory( void )
{
	boost::filesystem::path path;
	
#if defined(_WIN32)
	const char *const appdata = getenv("APPDATA");
	if (appdata != NULL)
	{
		path = boost::filesystem::path(appdata) / "Nanobounce";
	}
	else
	{
		path = boost::filesystem::current_path();
	}
#else
	const char *const xdg_config_home = getenv("XDG_CONFIG_HOME");
	if (xdg_config_home != NULL)
	{
		path = boost::filesystem::path(xdg_config_home) / "nanobounce";
	}
	else
	{
		const char *const home = getenv("HOME");
		if (home != NULL)
		{
			path = boost::filesystem::path(home) / ".config" / "nanobounce";
		}
		else
		{
			path = boost::filesystem::current_path();
		}
	}
#endif
	
	boost::filesystem::create_directories(path);
	
	return path;
}

std::vector<std::string> list_directories( const boost::filesystem::path &path )
{
	std::vector<std::string> entries;
	
	const boost::filesystem::directory_iterator end;
	for (auto i = boost::filesystem::directory_iterator(path); i != end; ++i)
		if (i->status().type() == boost::filesystem::file_type::directory_file)
			entries.push_back(i->path().filename().string());
	
	return entries;
}

std::vector<std::string> list_files( const boost::filesystem::path &path )
{
	std::vector<std::string> entries;
	
	const boost::filesystem::directory_iterator end;
	for (auto i = boost::filesystem::directory_iterator(path); i != end; ++i)
		if (i->status().type() == boost::filesystem::file_type::regular_file)
			entries.push_back(i->path().filename().string());
	
	return entries;
}

std::string sanitize_filename( const std::string &filename )
{
	std::string sanitized;
	
	for (char c : filename)
		if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
			sanitized += c;
		else
			sanitized += '_';
	
	return sanitized;
}
