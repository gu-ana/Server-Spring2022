#include <fstream>
#include <sstream>
#include <boost/range.hpp>

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
    std::ofstream out(p.string());
    out << body;
    return true;
}

bool RealFileSystem::read_file(const boost::filesystem::path& p, std::string& file)
{
    std::ifstream t(p.string());
    std::stringstream buffer;
    buffer << t.rdbuf();
    file = buffer.str();
    return true;
}

int RealFileSystem::get_unique_file_name(const boost::filesystem::path p)
{
    int largest_file = 0; // store index of largest file in directory
    for (const auto &entry : boost::make_iterator_range(boost::filesystem::directory_iterator(p), {}))
    {
        std::string curr_file_name = entry.path().stem().string();
        int curr_file_num = stoi(curr_file_name);
        if (curr_file_num > largest_file)
        {
            largest_file = curr_file_num;
        }
    }

    // Now that we have the largest file number in the directory, add 1 and add to file path
    return largest_file + 1;
}

std::string RealFileSystem::get_file_list(const boost::filesystem::path p)
{
    std::string result = "[";
    for (const auto &entry : boost::make_iterator_range(boost::filesystem::directory_iterator(p), {}))
    {
        std::string curr_file_name = entry.path().stem().string();
        result = result + curr_file_name + ", ";
    }
    result = result.substr(0, result.length() - 2); // extra ,_ at the end of string
    result = result + "]";
    return result;
}