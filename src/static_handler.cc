#include <boost/filesystem.hpp>
#include <string>
#include <iostream>

#include "static_handler.h"
#include "mime_types.h"
#include "logger.h"

int StaticHandler::handle_request(http::response<http::string_body>& httpResponse) 
{
    std::string target = std::string(httpRequest_.target()).c_str();
    LOG(info) << "Client requested " << target << '\n';
    std::string uri = longest_prefix_match(map_, target);
    LOG(info) << "Server remap target " << uri << '\n';
    
    uri.replace(0, 1, "../");
    boost::filesystem::path target_path(uri);
    std::ifstream fileStream(target_path.c_str(), std::ios::in | std::ios::binary);
    if(!boost::filesystem::exists(target_path) || !boost::filesystem::is_regular_file(target_path)) 
    {
        LOG(severity_level::error) << "HTTP Error 404 response created, Could not find target file";
        set_response(http::status::not_found, "text/plain", "File not found\n", httpResponse);
        return 2;
    }
    else if (!fileStream) 
    {
        LOG(severity_level::error) << "HTTP Error 404 response created, could not open file \n";
        set_response(http::status::not_found, "text/plain", "Cannot open file\n", httpResponse);
        return 2;
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
    
    return 0;
}

std::string StaticHandler::longest_prefix_match(std::map<std::string, std::string> map, std::string target)
{
    std::string uri = target;
    size_t max_match_size = 0;
    for(const auto& root: map) 
    {
        std::size_t found = target.find(root.first);
        if (found != std::string::npos) 
        {
            if(max_match_size < root.first.size()) 
            {
                uri = root.second + '/' + target.substr(found + root.first.size(), std::string::npos); 
                max_match_size = root.first.size();
            }
        }
    }
    return uri;

}

void StaticHandler::set_map(std::map<std::string, std::string> map)
{
    map_ = map;
}