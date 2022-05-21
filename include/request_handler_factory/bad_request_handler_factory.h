#ifndef BADREQUESTHANDLERFACTORY_H
#define BADREQUESTHANDLERFACTORY_H

#include "request_handler_factory/request_handler_factory.h"

class BadRequestHandlerFactory : public RequestHandlerFactory
{
    public:
        RequestHandler* create();
};

#endif