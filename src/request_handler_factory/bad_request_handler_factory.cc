#include "request_handler_factory/bad_request_handler_factory.h"
#include "request_handler/bad_request_handler.h"

RequestHandler* BadRequestHandlerFactory::create()
{
    return (new BadRequestHandler());
}