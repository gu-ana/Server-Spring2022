#include "request_handler/error_handler.h"
#include "logger.h"

bool ErrorHandler::handle_request(http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse)
{   
    set_response(http::status::not_found, "text/plain", "Could not handle request\n", httpResponse);
    return true;
}
