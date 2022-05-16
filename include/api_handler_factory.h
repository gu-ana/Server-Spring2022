#ifndef APIHANDLERFACTORY_H
#define APIHANDLERFACTORY_H

#include "request_handler_factory.h"

class ApiHandlerFactory : public RequestHandlerFactory 
{
    public:
        ApiHandlerFactory(std::string location, std::string data_path_);
        RequestHandler* create();
    private:
        std::string location_;
        std::string data_path_;
};

#endif