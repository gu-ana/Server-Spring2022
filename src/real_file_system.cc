#include <fstream>
#include <sstream>

#include "real_file_system.h"

bool RealFileSystem::exists(const boost::filesystem::path& p)
{
    return boost::filesystem::exists(p);
}

bool RealFileSystem::create_directory(const boost::filesystem::path& p)
{
    return boost::filesystem::create_directory(p);
}

bool RealFileSystem::is_regular_file(const boost::filesystem::path& p)
{
    return boost::filesystem::is_regular_file(p);
}

bool RealFileSystem::remove(const boost::filesystem::path& p)
{
    return boost::filesystem::remove(p);
}

bool RealFileSystem::write_file(const boost::filesystem::path& p, const std::string& body)
{
    std::ofstream out(p);
    out << body;
    return true;
}

bool RealFileSystem::read_file(const boost::filesystem::path& p, std::string& file)
{
    std::ifstream t(p);
    std::stringstream buffer;
    buffer << t.rdbuf();
    file = buffer.str();
    return true;
}