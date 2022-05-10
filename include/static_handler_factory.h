#ifndef STATICHANDLERFACTORY_H
#define STATICHANDLERFACTORY_H

#include "request_handler_factory.h"

class StaticHandlerFactory : public RequestHandlerFactory 
{
    public:
        StaticHandlerFactory(std::string location, std::string root_path);
        RequestHandler* create();
    private:
        std::string location_;
        std::string root_path_;
};

#endif