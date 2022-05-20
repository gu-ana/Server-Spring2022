#include "request_handler_factory/health_handler_factory.h"
#include "request_handler/health_handler.h"

RequestHandler* HealthHandlerFactory::create()
{
    return (new HealthHandler());
}