#include "request_handler_factory/palette_handler_factory.h"
#include "request_handler/palette_handler.h"

RequestHandler* PaletteHandlerFactory::create()
{
    return (new PaletteHandler());
}