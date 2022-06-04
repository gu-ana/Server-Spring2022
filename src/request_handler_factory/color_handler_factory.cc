#include "request_handler_factory/color_handler_factory.h"
#include "request_handler/color_handler.h"

RequestHandler* ColorHandlerFactory::create()
{
    return (new ColorHandler());
}