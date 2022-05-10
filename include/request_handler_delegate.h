#ifndef REQUESTHANDLERDELEGATE_H 
#define REQUESTHANDLERDELEGATE_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <string>
#include <map>
#include "request_handler_factory.h"

namespace http = boost::beast::http;

class RequestHandlerDelegate 
{
    public:
        void processRequest(std::string request_string, http::response<http::string_body>& httpResponse, std::string endpoint);
        void set_factory_routes(std::map<std::string, std::shared_ptr<RequestHandlerFactory>> factory_routes);
    private:
        std::map<std::string, std::shared_ptr<RequestHandlerFactory>> factory_routes_;
};

#endif