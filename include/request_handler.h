#ifndef REQUESTHANDLER_H 
#define REQUESTHANDLER_H 

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <string>
#include <map>

using boost::asio::ip::tcp;
namespace http = boost::beast::http;

class RequestHandler {
    public:
        bool set_request(std::string request_string);
        virtual int handle_request(http::response<http::string_body>& httpResponse) = 0;
        
    //protected because we want the derived classes to have access
    protected:
        std::string httpRequestString_;
        http::request<http::string_body> httpRequest_;
        http::response<http::string_body> httpResponse_;
};

#endif