#ifndef ECHOHANDLER_H 
#define ECHOHANDLER_H 

#include "request_handler.h"

class EchoHandler: public RequestHandler {
    public:
        int handle_request(http::response<http::string_body>& httpResponse);
    
};

#endif
