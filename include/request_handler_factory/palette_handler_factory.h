#ifndef PALETTEHANDLERFACTORY_H
#define PALETTEHANDLERFACTORY_H

#include "request_handler_factory/request_handler_factory.h"

class PaletteHandlerFactory : public RequestHandlerFactory 
{
    public:
        RequestHandler* create();
        
        
};

#endif