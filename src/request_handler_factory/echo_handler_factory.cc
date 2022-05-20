#include "request_handler_factory/echo_handler_factory.h"
#include "request_handler/echo_handler.h"

RequestHandler* EchoHandlerFactory::create()
{
    return (new EchoHandler());
}