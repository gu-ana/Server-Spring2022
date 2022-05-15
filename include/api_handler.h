#ifndef APIHANDLER_H 
#define APIHANDLER_H 

#include "request_handler.h"

class ApiHandler: public RequestHandler 
{
    public:
        ApiHandler(std::string location, std::string data_path);
        bool handle_request(http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse);
    private:
        std::string data_path_;
        std::string location_;
};

#endif
