#include <boost/filesystem.hpp>
#include <string>
#include <iostream>

#include "request_handler/request_handler.h"
#include "request_handler/static_handler.h"
#include "mime_types.h"
#include "logger.h"

StaticHandler::StaticHandler(std::string location, std::string root_file_path)
    :location_(location), root_file_path_(root_file_path){}

bool StaticHandler::handle_request(http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse)
{   
    std::string target = std::string(httpRequest.target()).c_str();
    LOG(info) << "Client requested " << target << '\n';

    // strip off location from front of target
    target.erase(0, location_.length());
 
    // prepend root_file_path
    target = root_file_path_ + "/" + target;
    target.replace(0, 1, "../");
    LOG(info) << "Server remap target " << target << '\n';
    
    boost::filesystem::path target_path(target);
    std::ifstream fileStream(target_path.c_str(), std::ios::in | std::ios::binary);
    if(!boost::filesystem::exists(target_path) || !boost::filesystem::is_regular_file(target_path)) 
    {
        LOG(severity_level::error) << "HTTP Error 404 response created, Could not find target file";
        set_response(http::status::not_found, "text/plain", "File not found\n", httpResponse);
        return false;
    }
    else if (!fileStream) 
    {
        LOG(severity_level::error) << "HTTP Error 404 response created, could not open file \n";
        set_response(http::status::not_found, "text/plain", "Cannot open file\n", httpResponse);
        return false;
    }
    else 
    {
        std::string extension = target_path.extension().string();   
        std::string body = "";
        char c;
        while (fileStream.get(c)) 
        {
            body += c;
        }
        fileStream.close();
        LOG(info) << "Requested file parsed successfully \n";
        set_response(http::status::ok, server::mime_types::extension_to_type(extension), body + '\n', httpResponse);
        LOG(info) << "HTTP response created successfully \n";
    }
    
    return true;
}