#ifndef SLEEPHANDLER_H 
#define SLEEPHANDLER_H 

#include "request_handler/request_handler.h"

class SleepHandler: public RequestHandler 
{
    public:
        bool handle_request(http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse);
        std::string getName();
    
};

#endif
