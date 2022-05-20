#ifndef HEALTHHANDLERFACTORY_H
#define HEALTHHANDLERFACTORY_H

#include "request_handler_factory/request_handler_factory.h"

class HealthHandlerFactory : public RequestHandlerFactory 
{
    public:
        RequestHandler* create();
        
        
};

#endif