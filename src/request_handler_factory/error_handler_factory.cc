#include "request_handler_factory/error_handler_factory.h"
#include "request_handler/error_handler.h"

RequestHandler* ErrorHandlerFactory::create()
{
    return (new ErrorHandler());
}