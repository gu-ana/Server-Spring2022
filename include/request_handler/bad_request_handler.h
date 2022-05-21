#ifndef BADREQUESTHANDLER_H 
#define BADREQUESTHANDLER_H 

#include "request_handler/request_handler.h"

class BadRequestHandler : public RequestHandler 
{
    public:
        bool handle_request(http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse);
};

#endif
