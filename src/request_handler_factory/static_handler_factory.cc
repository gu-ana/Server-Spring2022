
#include "request_handler_factory/static_handler_factory.h"
#include "request_handler/static_handler.h"
#include <iostream>

StaticHandlerFactory::StaticHandlerFactory(std::string location, std::string root_path)
  : location_(location), root_path_(root_path) 
{}

RequestHandler* StaticHandlerFactory::create()
{
    return (new StaticHandler(location_, root_path_));
}