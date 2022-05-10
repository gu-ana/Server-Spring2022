#include "error_handler_factory.h"
#include "error_handler.h"

RequestHandler* ErrorHandlerFactory::create()
{
    return (new ErrorHandler());
}