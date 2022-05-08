#include "request_handler_delegate.h"
#include "logger.h"
#include "request_handler.h"
#include "static_handler.h"
#include "echo_handler.h"
#include "error_handler.h"

bool parse_request(std::string httpRequestString, http::request<http::string_body>& httpRequest) 
{
  http::request_parser<http::string_body> req_parser;
  req_parser.eager(true);

  boost::system::error_code ec;
  req_parser.put(boost::asio::buffer(httpRequestString), ec);
  if(ec) 
  {
      LOG(severity_level::error) << "Could not parse request string: " << ec.message() << '\n' ; 
      return false;
  }
  httpRequest = req_parser.get();
  return true;
}

void RequestHandlerDelegate::processRequest(std::string request_string, http::response<http::string_body>& httpResponse, std::string endpoint)
{
    RequestHandler* handler = nullptr;
    http::request<http::string_body> httpRequest; 
    if (!parse_request(request_string, httpRequest))
    {
      handler = new ErrorHandler;
      LOG(info) << "HTTP response 400: Received bad request target\n";
    }
    
    std::string target = std::string(httpRequest.target()).c_str();
    
    
    //TODO @Will Make sure that if the target == "/", we  new the handler to ErrorHandler
    if(target == "/echo/" || target == "/echo")
    {
      handler = new EchoHandler;
      LOG(info) << "Echo request received from " << endpoint << "\n";
    }
    
    // else if (target.find("/static") != std::string::npos)
    // {
    //   handler = new StaticHandler;
    //   // LOG(info) << "Static request received from " << socket_.remote_endpoint() << "\n";
    //   ((StaticHandler*) handler)->set_map(map_);
    // }

    if(handler == nullptr)
    {
      LOG(info) << "HTTP response 400: Received bad request target from " << endpoint << "\n";
    }
    else
    {
      handler->handle_request(httpRequest, httpResponse);
    }

    delete handler;
}

void RequestHandlerDelegate::set_map(std::map<std::string, std::string> map)
{
    map_ = map;
}