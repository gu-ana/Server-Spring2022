#include "request_handler_delegate.h"
#include "logger.h"
#include "static_handler.h"
#include "echo_handler.h"

std::string get_target(std::string httpRequestString) 
{
  http::request_parser<http::string_body> req_parser;
    req_parser.eager(true);

    boost::system::error_code ec;
    req_parser.put(boost::asio::buffer(httpRequestString), ec);
    if(ec) 
    {
        LOG(severity_level::error) << "Could not parse request string: " << ec.message() << '\n' ; 
        return "";
    }
    return std::string(req_parser.get().target()).c_str();
}

void set_response(boost::beast::http::status status, std::string content_type, std::string body, http::response<http::string_body>& httpResponse)
{
  httpResponse.version(11);
  httpResponse.result(status);
  httpResponse.set(http::field::content_type, content_type);
  httpResponse.body() = body;
  httpResponse.prepare_payload();
}

void RequestHandlerDelegate::processRequest(std::string request_string, http::response<http::string_body>& httpResponse, std::string endpoint)
{
    std::string target = get_target(request_string);
    RequestHandler* handler = nullptr;

    if(target == "/echo/" || target == "/echo")
    {
      handler = new EchoHandler;
      LOG(info) << "Echo request received from " << endpoint << "\n";
    }
    else if (target.find("/static") != std::string::npos)
    {
      handler = new StaticHandler;
      LOG(info) << "Static request received from " << endpoint << "\n";
      ((StaticHandler*) handler)->set_map(map_);
    }

    if(handler == nullptr)
    {
      set_response(http::status::bad_request, "text/plain", "Bad Request\n", httpResponse);
      LOG(info) << "HTTP response 400: Received bad request target from " << endpoint << "\n";
    }
    else
    {
      handler->set_request(request_string);
      handler->handle_request(httpResponse);
    }

    delete handler;
}

void RequestHandlerDelegate::set_map(std::map<std::string, std::string> map)
{
    map_ = map;
}