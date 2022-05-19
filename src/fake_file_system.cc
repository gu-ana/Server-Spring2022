#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

#include "fake_file_system.h"

int FakeFileSystem::get_unique_file_name(const boost::filesystem::path p)
{
    int largest_file = 0;
    for (auto file_path : ffs)
    {
        if (file_path == p.string()) continue;
        // find unique file name in ffs where prefix matches
        int i = file_path.find_last_of("/");
        std::string prefix = file_path.substr(0, i);
        std::string id = file_path.substr(i+1);
        if (prefix == p.string())
        {
            int cur_file_num = stoi(id);
            if (cur_file_num > largest_file)
            {
                largest_file = cur_file_num;
            }
        }
    }
    return largest_file + 1;
}

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

bool FakeFileSystem::write_file(const boost::filesystem::path& filePath, const std::string& body)
{   
    if (!exists(filePath.parent_path()))
    {
        create_directory(filePath.parent_path());
    }

    auto r = ffs.insert(filePath.string());
    
    return true;
}

int FakeFileSystem::gen_id_and_write_file(const boost::filesystem::path& dirPath, const std::string& body)
{
    if (!exists(dirPath))
    {
        create_directory(dirPath);
    }
    
    int id = get_unique_file_name(dirPath);
    boost::filesystem::path filePath(dirPath.string() + "/" + std::to_string(id));

    auto r = ffs.insert(filePath.string());

    return id;
}

bool FakeFileSystem::read_file(const boost::filesystem::path& p, std::string& file)
{
    if (exists(p.string())) 
    {
        file = "Read file successfully\n";
        return true;
    }
    else 
    {
        return false;
    } 
}

std::string FakeFileSystem::get_file_list(const boost::filesystem::path p)
{
    std::string result = "[";
    // get entity from p
    // look through ffs for matchin prefix and add it to json
    for (auto file_path : ffs) 
    {
        if (file_path == p.string()) continue;
        int i = file_path.find_last_of("/");
        std::string prefix = file_path.substr(0, i);
        std::string id = file_path.substr(i+1);
        if (prefix == p.string())
        {
            result = result + id + ", ";
        }
    }
    result = result.substr(0, result.length() - 2); // extra ,_ at the end of string
    result = result + "]";
    return result;
}
