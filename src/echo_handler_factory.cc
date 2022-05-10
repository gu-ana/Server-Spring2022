#include "echo_handler_factory.h"
#include "echo_handler.h"

RequestHandler* EchoHandlerFactory::create()
{
    return (new EchoHandler());
}