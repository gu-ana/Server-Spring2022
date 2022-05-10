#ifndef ERRORHANDLERFACTORY_H
#define ERRORHANDLERFACTORY_H

#include "request_handler_factory.h"

class ErrorHandlerFactory : public RequestHandlerFactory 
{
    public:
        RequestHandler* create();
        
};

#endif