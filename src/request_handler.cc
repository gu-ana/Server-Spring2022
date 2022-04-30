#include <boost/filesystem.hpp>
#include <string>
#include <iostream>

#include "request_handler.h"
#include "logger.h"

bool RequestHandler::set_request(std::string request_string)
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