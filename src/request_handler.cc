#include <boost/filesystem.hpp>
#include <string>
#include <iostream>

#include "request_handler.h"
#include "mime_types.h"
#include "logger.h"

bool RequestHandler::format_request(std::string request_string) 
{
    http::request_parser<http::string_body> req_parser;
    req_parser.eager(true);

    boost::system::error_code ec;
    req_parser.put(boost::asio::buffer(request_string), ec);
    if(ec)
    {
        LOG(severity_level::error) << "Could not parse request string: " << ec.message() << "\n";
        return false;
    }
    httpRequestString_ = request_string;
    httpRequest_ = req_parser.get(); 
    return true;
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
int StaticHandler::handle_request(http::response<http::string_body>& httpResponse) 
{
    std::string target = std::string(httpRequest_.target()).c_str();
    LOG(info) << "Client requested " << target << '\n';
    std::string uri = longest_prefix_match(map_, target);
    LOG(info) << "Server remap target " << uri << '\n';
    
    uri.replace(0, 1, "../");
    boost::filesystem::path target_path(uri);
    std::ifstream f(target_path.c_str(), std::ios::in | std::ios::binary);
    if(!boost::filesystem::exists(target_path) || !boost::filesystem::is_regular_file(target_path)) 
    {
        LOG(severity_level::error) << "HTTP Error 404 response created, Could not find target file";
        httpResponse_.version(11);
        httpResponse.result(http::status::not_found);
        httpResponse.set(http::field::content_type, "text/plain");
        httpResponse.body() = "File not found";
        httpResponse.prepare_payload();
        return 2;
    }
    else if (!f) 
    {
        LOG(severity_level::error) << "HTTP Error 404 response created, could not open file \n";
        httpResponse_.version(11);
        httpResponse.result(http::status::not_found);
        httpResponse.set(http::field::content_type, "text/plain");
        httpResponse.body() = "Cannot Open File";
        httpResponse.prepare_payload();
        return 2;
    }
    else 
    {
        std::string extension = target_path.extension().string();   
        std::string body = "";
        char c;
        while (f.get(c)) 
        {
            body += c;
        }
        f.close();
        LOG(info) << "Requested file parsed successfully \n";
        httpResponse.version(11);
        httpResponse.result(http::status::ok);
        httpResponse.set(http::field::content_type, server::mime_types::extension_to_type(extension)); 
        httpResponse.body() = body + '\n';
        httpResponse.prepare_payload();
        LOG(info) << "HTTP response created successfully \n";
    }
    
    return 0;
}

int EchoHandler::handle_request(http::response<http::string_body>& httpResponse) 
{
    httpResponse.version(11);
    httpResponse.result(http::status::ok);
    httpResponse.set(http::field::content_type, "text/plain");
    httpResponse.body() = httpRequestString_;
    httpResponse.prepare_payload();

    return 0;
}

void StaticHandler::set_map(std::map<std::string, std::string> map)
{
    map_ = map;
}