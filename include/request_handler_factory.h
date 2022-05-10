#ifndef REQUESTHANDLERFACTORY_H
#define REQUESTHANDLERFACTORY_H

#include <map>
#include <string>
#include "request_handler.h"

class RequestHandlerFactory 
{
    public: 
        virtual RequestHandler* create() = 0;

};

#endif