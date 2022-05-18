#ifndef SLEEPHANDLERFACTORY_H
#define SLEEPHANDLERFACTORY_H

#include "request_handler_factory.h"

class SleepHandlerFactory : public RequestHandlerFactory 
{
    public:
        RequestHandler* create();
        
        
};

#endif