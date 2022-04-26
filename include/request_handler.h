#ifndef REQUESTHANDLER_H 
#define REQUESTHANDLER_H 

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <string>


using boost::asio::ip::tcp;
namespace http = boost::beast::http;



class RequestHandler {
    public:
        bool format_request(std::string request_string);
        virtual int handle_request(http::response<http::string_body>& httpResponse) = 0;
        http::response<http::string_body> get_response();
        
    //protected because we want the derived classes to have access
    protected:
        std::string httpRequestString_;
        http::request<http::string_body> httpRequest_;
        http::response<http::string_body> httpResponse_;
};

class StaticHandler: public RequestHandler {
    public:
        void set_map(std::map<std::string, std::string> map);
        int handle_request(http::response<http::string_body>& httpResponse);
    private:
        std::map<std::string, std::string> map_;
};

class EchoHandler: public RequestHandler {
    public:
        int handle_request(http::response<http::string_body>& httpResponse);
    
};
#endif