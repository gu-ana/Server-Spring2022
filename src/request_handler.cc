#include <boost/filesystem.hpp>
#include <string>
#include <iostream>

#include "request_handler.h"
#include "logger.h"

void RequestHandler::set_response(boost::beast::http::status status, std::string content_type, std::string body, http::response<http::string_body>& httpResponse)
{
  httpResponse.version(11);
  httpResponse.result(status);
  httpResponse.set(http::field::content_type, content_type);
  httpResponse.body() = body;
  httpResponse.prepare_payload();
}