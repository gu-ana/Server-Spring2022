#include "request_handler_delegate.h"
#include "logger.h"
#include "request_handler/request_handler.h"
#include "request_handler/static_handler.h"
#include "request_handler/echo_handler.h"
#include "request_handler/error_handler.h"
#include "request_handler/health_handler.h"
#include "request_handler/bad_request_handler.h"
#include "request_handler_factory/request_handler_factory.h"

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

std::string longest_prefix_matching(std::map<std::string, std::shared_ptr<RequestHandlerFactory>> factory_routes, std::string uri)
{
    std::string matched_location_path = "/";  // "/" path represents "errorhandler"
    size_t max_match_size = 0;
    for(const auto& root: factory_routes) 
    {
        std::string uri_substr = uri.substr(0, root.first.size());
        if (root.first == uri_substr) 
        {
            if(max_match_size < root.first.size()) 
            {
                matched_location_path = root.first; // return the matched routes.key location
                max_match_size = root.first.size();
            }
        }
    }
    return matched_location_path;
}

void RequestHandlerDelegate::processRequest(std::string request_string, http::response<http::string_body>& httpResponse, std::string endpoint)
{
    std::string matched_location_path = "/";  // this is the default case of 404Handler
    http::request<http::string_body> httpRequest; 
    if (!parse_request(request_string, httpRequest))
    {
      matched_location_path = "bad request"; // matches to bad request handler
    }
    else 
    {
      std::string target = std::string(httpRequest.target()).c_str();
      LOG(info) << "Successfully extracted request URI, " << target << ".\n";
      // longest prefix matching here for uri against routes.keys (location path)
      matched_location_path = longest_prefix_matching(factory_routes_, target);
    }
    
    std::shared_ptr<RequestHandlerFactory> factory = factory_routes_[matched_location_path];
    RequestHandler* handler = factory->create();
    LOG(info) << "Created request handler using factory.\n";

    bool status = handler->handle_request(httpRequest, httpResponse);
    if (status) 
    {
        LOG(info) << "[ResponseMetric] ResponseCode: " << std::to_string(httpResponse.result_int()) 
                                  << " RequestPath: " << httpRequest.target()
                                  << " MatchedRequestHandler: " << handler->getName()
                                  << " RequestIP: " << endpoint
                                  << " Message: Successfully handled request.\n";        
    }
    else 
    {
        LOG(info) << "[ResponseMetric] ResponseCode: " << std::to_string(httpResponse.result_int())
                                  << " RequestPath: " << httpRequest.target()
                                  << " MatchedRequestHandler: " << handler->getName()
                                  << " RequestIP: " << endpoint
                                  << " Message: Unable to handle request.\n";     
    }

    delete handler;
}

void RequestHandlerDelegate::set_factory_routes(std::map<std::string, std::shared_ptr<RequestHandlerFactory>> factory_routes)
{
    factory_routes_ = factory_routes;
}