#include <fstream>
#include <sstream>

#include "fake_file_system.h"

bool FakeFileSystem::exists(const boost::filesystem::path& p)
{
    return ffs.find(p.string()) != ffs.end();
}

bool FakeFileSystem::create_directory(const boost::filesystem::path& p)
{
    auto r = ffs.insert(p.string());
    return r.second; // r.first is an iterator which points at inserted element, r.second is true if inserted, false if already exists
}

bool FakeFileSystem::is_regular_file(const boost::filesystem::path& p)
{
    return true;
}

bool FakeFileSystem::remove(const boost::filesystem::path& p)
{
    return ffs.erase(p.string());
}

bool FakeFileSystem::write_file(const boost::filesystem::path& p, const std::string& body)
{
    auto r = ffs.insert(p.string());
    return r.second;
}

bool FakeFileSystem::read_file(const boost::filesystem::path& p, std::string& file)
{
    if (exists(p)) return true;
    else return false;
}
