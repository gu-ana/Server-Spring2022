#include "echo_handler.h"
#include "logger.h"

int EchoHandler::handle_request(http::response<http::string_body>& httpResponse) 
{
    httpResponse.version(11);
    httpResponse.result(http::status::ok);
    httpResponse.set(http::field::content_type, "text/plain");
    httpResponse.body() = httpRequestString_;
    httpResponse.prepare_payload();

    return 0;
}