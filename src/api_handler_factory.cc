
#include "api_handler_factory.h"
#include "api_handler.h"
#include <iostream>

ApiHandlerFactory::ApiHandlerFactory(std::string location, std::string data_path)
  : location_(location), data_path_(data_path) 
{}

RequestHandler* ApiHandlerFactory::create()
{
    return (new ApiHandler(location_, data_path_));
}