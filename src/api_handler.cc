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

ApiHandler::ApiHandler(std::string location, std::string data_path)
    :location_(location), data_path_(data_path){}

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

int getUniqueFileName(boost::filesystem::path p) 
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

// Creates file and returns the file number created
int handlePost(http::request<http::string_body> httpRequest, vector<std::string> uri)
{
    std::string filePath = data_path_ + extractEntity(uri);
    std::string postBody = httpRequest.body();
    
        
    boost::filesystem::path p(filePath);
    if (boost::filesystem::exists(p) == false)
    {
        boost::filesystem::create_directory(p);
    }
        
    int created_file = getUniqueFileName(p);

    filePath = filePath + "/" + std::to_string(created_file);
    ofstream myfile;
    myfile.open(filePath);
    myfile << postBody;
    myfile.close();

    return created_file;
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

    if (method == "POST") 
    {
        // CREATE
        if (uri.size() != 1)
        {
            LOG(error) << "POST request has more than one entity, please post to /api/{entity} only.";
            set_response(http::status::bad_request, "text/plain", "Not a valid API request target for POST\n", httpResponse);
            return false;
        }
        int created_file_number = handlePost(httpRequest, uri);
        std::string json_response = "{\"id\": " + to_string(created_file_number) + "}";
        set_response(http::status::ok, "text/plain", json_response, httpResponse);
    }
    else if (method == "GET") 
    {
        // RETRIEVE & LIST
    }
    else if (method == "PUT") 
    {
        // UPDATE

    }
    else if (method == "DELETE") 
    {
        // DELETE

    }
    else
    {
        return false;
    }

    
    return true;
}