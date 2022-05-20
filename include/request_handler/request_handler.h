#ifndef REQUESTHANDLER_H 
#define REQUESTHANDLER_H 

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <string>
#include <map>

using boost::asio::ip::tcp;
namespace http = boost::beast::http;

class RequestHandler 
{
    public:
        virtual bool handle_request(http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse) = 0;
    
    //protected because we want the derived classes to have access
    protected:
         void set_response(boost::beast::http::status status, std::string content_type, std::string body, http::response<http::string_body>& httpResponse);
};

#endif