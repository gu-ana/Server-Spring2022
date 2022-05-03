#include "echo_handler.h"
#include "logger.h"

int EchoHandler::handle_request(http::response<http::string_body>& httpResponse) 
{
	set_response(http::status::ok, "text/plain", httpRequestString_, httpResponse);
    return 0;
}