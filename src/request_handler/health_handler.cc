#include "request_handler/health_handler.h"
#include "logger.h"

bool HealthHandler::handle_request(http::request<http::string_body> httpRequest, http::response<http::string_body>& httpResponse)
{   
	set_response(http::status::ok, "text/plain", "OK\n", httpResponse);

    return true;
}

std::string HealthHandler::getName()
{
    return std::string("HealthHandler");
}