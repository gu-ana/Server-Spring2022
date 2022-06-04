#include "request_handler/color_handler.h"
#include "request_handler/request_handler.h"
#include "logger.h"
#include "mime_types.h"

#include <boost/filesystem.hpp>
#include <string>
#include <iostream>


bool ColorHandler::handle_request(http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse)
{   
    std::string target = "../frontend/build/index.html";
    
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

std::string ColorHandler::getName()
{
    return std::string("ColorHandler");
}