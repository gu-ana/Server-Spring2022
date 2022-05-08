#include "error_handler.h"
#include "logger.h"

bool ErrorHandler::handle_request(http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse)
{   
    std::stringstream httpRequestStream;
    httpRequestStream << httpRequest;
    
    set_response(http::status::not_found, "text/plain", "Bad Request\n", httpResponse);

    return true;
}
