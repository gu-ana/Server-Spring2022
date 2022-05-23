#include "request_handler/bad_request_handler.h"
#include "logger.h"

bool BadRequestHandler::handle_request(http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse)
{   
    set_response(http::status::bad_request, "text/plain", "Bad Request\n", httpResponse);
    return true;
}

std::string BadRequestHandler::getName()
{
    return std::string("BadRequestHandler");
}