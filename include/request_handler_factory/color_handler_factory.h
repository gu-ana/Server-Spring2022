#ifndef COLORHANDLERFACTORY_H
#define COLORHANDLERFACTORY_H

#include "request_handler_factory/request_handler_factory.h"

class ColorHandlerFactory : public RequestHandlerFactory 
{
    public:
        RequestHandler* create();
        
        
};

#endif