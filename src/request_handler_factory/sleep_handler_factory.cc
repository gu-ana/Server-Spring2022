#include "request_handler_factory/sleep_handler_factory.h"
#include "request_handler/sleep_handler.h"

RequestHandler* SleepHandlerFactory::create()
{
    return (new SleepHandler());
}