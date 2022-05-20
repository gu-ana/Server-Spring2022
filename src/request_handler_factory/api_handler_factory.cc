#include "request_handler_factory/api_handler_factory.h"
#include "request_handler/api_handler.h"
#include <iostream>

ApiHandlerFactory::ApiHandlerFactory(std::string location, std::string data_path, std::shared_ptr<FileSystem> fs)
  : location_(location), data_path_(data_path), fs_(fs)
{}

RequestHandler* ApiHandlerFactory::create()
{
    return (new ApiHandler(location_, data_path_, fs_));
}