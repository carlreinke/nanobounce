#ifndef FILE_SYSTEM_HPP
#define FILE_SYSTEM_HPP

extern boost::filesystem::path user_data_directory;

extern std::vector<std::string> list_directories( const boost::filesystem::path &path );

extern std::vector<std::string> list_files( const boost::filesystem::path &path );

extern std::string sanitize_filename( const std::string &filename );

#endif // FILE_SYSTEM_HPP
