#ifndef HEALTHHANDLER_H 
#define HEALTHHANDLER_H 

#include "request_handler/request_handler.h"

class HealthHandler: public RequestHandler 
{
    public:
        bool handle_request(http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse);
    
};

#endif
