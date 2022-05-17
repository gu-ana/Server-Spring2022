#ifndef FILE_SYSTEM_H 
#define FILE_SYSTEM_H 

#include <boost/filesystem.hpp>
#include <string>
#include <set>

class FileSystem {
    public: 
        virtual bool exists(const boost::filesystem::path& p) = 0;
        virtual bool create_directory(const boost::filesystem::path& p) = 0;
        virtual bool is_regular_file(const boost::filesystem::path& p) = 0;
        virtual bool remove(const boost::filesystem::path& p) = 0;
        virtual bool write_file(const boost::filesystem::path& p, const std::string& body) = 0;
        virtual bool read_file(const boost::filesystem::path& p, std::string& file) = 0;
        virtual int get_unique_file_name(const boost::filesystem::path p) = 0;
        virtual std::string get_file_list(const boost::filesystem::path p) = 0;
};

#endif