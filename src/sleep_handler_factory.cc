#include "sleep_handler_factory.h"
#include "sleep_handler.h"

RequestHandler* SleepHandlerFactory::create()
{
    return (new SleepHandler());
}