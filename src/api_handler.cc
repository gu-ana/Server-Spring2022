#include <boost/filesystem.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>

#include "request_handler.h"
#include "api_handler.h"
#include "mime_types.h"
#include "logger.h"

using namespace std;

ApiHandler::ApiHandler(std::string location, std::string data_path, std::shared_ptr<FileSystem> fs)
    :location_(location), data_path_(data_path), fs_(fs){}

// split function helps break up the uri into more manageable parts
vector<string> split(const string &s, char delim) 
{
    vector<string> result;
    stringstream ss (s);
    string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    // remove all empty strings
    result.erase(std::remove(result.begin(), result.end(), ""), result.end());
    result.erase(result.begin()); // remove the api

    return result;
}

std::string extractEntity(vector<string> uri) 
{
    return "/" + uri[0];
}

std::string extractFileName(vector<string> uri)
{
    return "/" + uri[1];
}

// Creates file and returns the file number created
bool ApiHandler::handlePost(vector<std::string> uri, http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse)
{
    if (uri.size() != 1)
    {
        LOG(error) << "POST request has more than one entity, please post to /api/{entity} only.";
        set_response(http::status::bad_request, "text/plain", "Not a valid API request target for POST\n", httpResponse);
        return false;
    }

    std::string filePath = data_path_ + extractEntity(uri);
    std::string postBody = httpRequest.body();
        
    boost::filesystem::path p(filePath);
    if (fs_->exists(p) == false)
    {
        fs_->create_directory(p);
    }

    int created_file = fs_->get_unique_file_name(p);

    filePath = filePath + "/" + std::to_string(created_file);
    boost::filesystem::path fp(filePath);
    LOG(info) << "Writing data to server file system at " << fp.string();

    fs_->write_file(fp, postBody);
    std::string json_response = "{\"id\": " + to_string(created_file) + "}";
    set_response(http::status::ok, "text/plain", json_response, httpResponse);

    return true;
}

std::string ApiHandler::extractFileContents(std::string fileName)
{
    std::string file_contents;
    boost::filesystem::path filePath(fileName);
    fs_->read_file(fileName, file_contents);
    return file_contents;
}

bool ApiHandler::handleGet(vector<std::string> uri, http::response<http::string_body>& httpResponse) 
{
    std::string filePath = data_path_ + extractEntity(uri);
    boost::filesystem::path p(filePath);
    if (fs_->exists(p) == false)
    {
        LOG(error) << "Cannot find directory " << extractEntity(uri);
        set_response(http::status::bad_request, "text/plain", "Invalid entity requested\n", httpResponse);
        return false;
    }
    if (uri.size() == 1)
    {
        // List
        std::string json_list = fs_->get_file_list(p);
        set_response(http::status::ok, "text/plain", json_list, httpResponse);
    }
    else if (uri.size() == 2)
    {
        // Retrieve
        std::string fileName = filePath + extractFileName(uri);
        std::string file_contents;
        bool read_success = fs_->read_file(fileName, file_contents);
        if (read_success) 
        {
            set_response(http::status::ok, "text/plain", file_contents, httpResponse);
        }
        else
        {
            LOG(error) << "Queried file does not exist";
            set_response(http::status::bad_request, "text/plain", "Requested ID does not exist\n", httpResponse);
            return false;
        }
    }
    else 
    {
        LOG(error) << "GET request has more than two entities, please post to /api/{entity}/[file] only.";
        set_response(http::status::bad_request, "text/plain", "Not a valid API request target for GET\n", httpResponse);
        return false;
    }    
    return true;
}

bool ApiHandler::handleDelete(vector<string> uri, http::response<http::string_body>& httpResponse) 
{
    if (uri.size() != 2)
    {
        LOG(error) << "DELETE request has incorrect number of arguments, should be /api/{entity}/{id}.";
        set_response(http::status::bad_request, "text/plain", "Not a valid API request target for DELETE\n", httpResponse);
        return false;
    }
    
    std::string fileName = data_path_ + extractEntity(uri) + extractFileName(uri);
    if (!fs_->remove(fileName))
    {
        LOG(error) << "Could not delete ID " << fileName;
        set_response(http::status::bad_request, "text/plain", "Could not delete requested ID\n", httpResponse);
        return false;
    }
    else
    {
        set_response(http::status::ok, "text/plain", "Successfully deleted ID\n", httpResponse);
        return true;
    }
}

bool ApiHandler::handlePut(vector<std::string> uri, http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse)
{
    if (uri.size() != 2)
    {
        LOG(error) << "PUT request has more than one entity, please post to /api/{entity} only.";
        set_response(http::status::bad_request, "text/plain", "Not a valid API request target for PUT\n", httpResponse);
        return false;
    }

    std::string filePath = data_path_ + extractEntity(uri);
    std::string postBody = httpRequest.body();
    
        
    boost::filesystem::path p(filePath);
    if (fs_->exists(p) == false)
    {
        fs_->create_directory(p);
    }
        
    std::string modified_file = uri[1];

    filePath = filePath + "/" + modified_file;
    boost::filesystem::path fpath(filePath);
    fs_->write_file(fpath, postBody);
    std::string json_response = "{\"id\": " + modified_file + "}";
    set_response(http::status::ok, "text/plain", json_response, httpResponse);

    return true;
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

bool isFileNumber(vector<string> uri) 
{
    if (uri.size() == 2)
    {
        std::string file_no = uri[1];
        return is_number(file_no);
    }
    return true;
}


bool ApiHandler::handle_request(http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse)
{   
    std::string target = std::string(httpRequest.target()).c_str();
    LOG(info) << "Client requested " << target << '\n';

    std::string method = httpRequest.method_string().to_string();

    // switch action based on method
    // target is the URI we need

    // /api/Shoes --> {shoes in our uri} 
    vector<string> uri = split(target,'/');

    bool validFile = isFileNumber(uri);
    if (validFile == false)
    {
        LOG(error) << "The requested file is not a number\n";
        set_response(http::status::bad_request, "text/plain", "Requested file must be a number\n", httpResponse);
        return false;
    }

    if (method == "POST") 
    {
        // CREATE
        bool post_result = handlePost(uri, httpRequest, httpResponse);
        return post_result;
    }
    else if (method == "GET") 
    {
        // RETRIEVE & LIST
        bool get_result = handleGet(uri, httpResponse);
        return get_result;
    }
    else if (method == "PUT") 
    {
        // UPDATE
        bool put_result = handlePut(uri, httpRequest, httpResponse);
        return put_result;
    }
    else if (method == "DELETE") 
    {
        // DELETE
        bool delete_result = handleDelete(uri, httpResponse);
        return delete_result;
    }
    else
    {
        return false;
    }
}