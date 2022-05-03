#ifndef REQUESTHANDLERDELEGATE_H 
#define REQUESTHANDLERDELEGATE_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <string>
#include <map>

namespace http = boost::beast::http;

// implemented in requesthandler.cc
void set_response(boost::beast::http::status status, std::string content_type, std::string body, http::response<http::string_body>& httpResponse);

class RequestHandlerDelegate 
{
    public:
        void processRequest(std::string request_string, 
        					http::response<http::string_body>& httpResponse, std::string endpoint);
        void set_map(std::map<std::string, std::string> map);
    private:
        std::map<std::string, std::string> map_;
};

#endif