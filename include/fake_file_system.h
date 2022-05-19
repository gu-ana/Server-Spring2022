#ifndef FAKE_FILE_SYSTEM_H 
#define FAKE_FILE_SYSTEM_H 

#include "file_system.h"

class FakeFileSystem : public FileSystem {
    public: 
        bool exists(const boost::filesystem::path& p);
        bool create_directory(const boost::filesystem::path& p);
        bool is_regular_file(const boost::filesystem::path& p);
        bool remove(const boost::filesystem::path& p);
        bool write_file(const boost::filesystem::path& filePath, const std::string& body);
        int gen_id_and_write_file(const boost::filesystem::path& dirPath, const std::string& body);
        bool read_file(const boost::filesystem::path& p, std::string& file);
        std::string get_file_list(const boost::filesystem::path p);
    private:
        std::set<std::string> ffs; // contains all files in dir and their paths
        int get_unique_file_name(const boost::filesystem::path p);

};

#endif