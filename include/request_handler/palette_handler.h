#ifndef PALETTEHANDLER_H 
#define PALETTEHANDLER_H 

#include "request_handler/request_handler.h"

class PaletteHandler: public RequestHandler 
{
    public:
        bool handle_request(http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse);
        std::string getName() override;
    
};

#endif