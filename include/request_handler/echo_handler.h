#ifndef ECHOHANDLER_H 
#define ECHOHANDLER_H 

#include "request_handler/request_handler.h"

class EchoHandler: public RequestHandler 
{
    public:
        bool handle_request(http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse);
        std::string getName();
    
};

#endif
