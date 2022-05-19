#include <fstream>
#include <sstream>
#include <boost/range.hpp>

#include "real_file_system.h"
#include "logger.h"

std::mutex RealFileSystem::file_creation_mutex_;

int get_unique_file_name(const boost::filesystem::path p)
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
    bool success = boost::filesystem::remove(p);

    // if directory is empty, delete it
    if (boost::filesystem::is_empty(p.parent_path())) {
        boost::filesystem::remove_all(p.parent_path());
    }
    return success;
}

bool RealFileSystem::write_file(const boost::filesystem::path& filePath, const std::string& body)
{
    // create directory if necessary
    if (boost::filesystem::exists(filePath.parent_path()) == false)
    {
        boost::filesystem::create_directory(filePath.parent_path());
    }
    
    // create file if necessary
    std::unique_lock<std::mutex> ul(file_creation_mutex_);
    FILE* fp = fopen(filePath.c_str(), "w"); // opens file if it already exists, otherwise creates it

    // write to file
    if (fp) 
    {
        flockfile(fp);
        file_creation_mutex_.unlock();
        LOG(info) << "Writing data to server file system at " << filePath.string();
        fputs(body.c_str(), fp);
        fflush(fp);
        fclose(fp);
        funlockfile(fp);
        return true;
    }
    else 
    {
        file_creation_mutex_.unlock();
        LOG(error) << "Unable to create file at " << filePath.string();
        return false;
    }
}

int RealFileSystem::gen_id_and_write_file(const boost::filesystem::path& dirPath, const std::string& body)
{
    // create directory if necessary
    if (boost::filesystem::exists(dirPath) == false)
    {
        boost::filesystem::create_directory(dirPath);
    }
    
    // generate ID and create file
    std::unique_lock<std::mutex> ul(file_creation_mutex_);
    int file_name = get_unique_file_name(dirPath);
    std::string filePath = dirPath.string() + "/" + std::to_string(file_name);
    FILE* fp = fopen(filePath.c_str(), "w"); // creates new file

    // write to file
    if (fp) 
    {
        flockfile(fp);
        file_creation_mutex_.unlock();
        LOG(info) << "Writing data to server file system at " << filePath;
        fputs(body.c_str(), fp);
        fflush(fp);
        fclose(fp);
        funlockfile(fp);
        return file_name;
    }
    else 
    {
        file_creation_mutex_.unlock();
        LOG(error) << "Unable to create file at " << filePath;
        return -1;
    }
}

bool RealFileSystem::read_file(const boost::filesystem::path& p, std::string& file)
{
    if (boost::filesystem::is_regular_file(p)) 
    {
        std::ifstream t(p.string());
        std::stringstream buffer;
        buffer << t.rdbuf();
        file = buffer.str();
        return true;
    }
    else 
    {
        return false;
    }
    
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