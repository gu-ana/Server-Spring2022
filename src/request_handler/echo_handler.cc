#include "request_handler/echo_handler.h"
#include "logger.h"

bool EchoHandler::handle_request(http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse)
{   
    std::stringstream httpRequestStream;
    httpRequestStream << httpRequest;
    
    set_response(http::status::ok, "text/plain", httpRequestStream.str(), httpResponse);

    return true;
}
