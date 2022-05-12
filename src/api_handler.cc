#include <boost/filesystem.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "request_handler.h"
#include "api_handler.h"
#include "mime_types.h"
#include "logger.h"

using namespace std;

ApiHandler::ApiHandler(std::string location, std::string root_file_path)
    :location_(location), root_file_path_(root_file_path){}

// split function helps break up the uri into more manageable parts
vector<string> split (const string &s, char delim) 
{
    vector<string> result;
    stringstream ss (s);
    string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
}


bool ApiHandler::handle_request(http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse)
{   
    std::string target = std::string(httpRequest.target()).c_str();
    LOG(info) << "Client requested " << target << '\n';

    std::string method = httpRequest.method_string().to_string();

    // switch action based on method
    // target is the URI we need

    // /api/Shoes --> {,api,shoes} (note that size is 3 not 2)
    vector<string> uri = split(target,'/');

    if (method == "POST") 
    {
        // CREATE


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