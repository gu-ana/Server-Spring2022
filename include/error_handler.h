#ifndef ERRORHANDLER_H 
#define ERRORHANDLER_H 

#include "request_handler.h"

class ErrorHandler : public RequestHandler 
{
    public:
        bool handle_request(http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse);
    
};

#endif
