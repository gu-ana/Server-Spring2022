#ifndef ECHOHANDLERFACTORY_H
#define ECHOHANDLERFACTORY_H

#include "request_handler_factory.h"

class EchoHandlerFactory : public RequestHandlerFactory 
{
    public:
        RequestHandler* create();
        
        
};

#endif