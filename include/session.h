#ifndef SESSION_H
#define SESSION_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <string>

#include "config_parser.h"
#include "logger.h"
#include "request_handler/request_handler.h"
#include "request_handler/echo_handler.h"
#include "request_handler/static_handler.h"
#include "request_handler_delegate.h"
#include "request_handler_factory/request_handler_factory.h"

using boost::asio::ip::tcp;
namespace http = boost::beast::http;

std::string get_target(std::string httpRequestString);

class Session {
    public:
        // functions
        Session(boost::asio::io_service& io_service, std::map<std::string, std::shared_ptr<RequestHandlerFactory>> factory_routes);
        virtual void start();
        virtual tcp::socket& socket();
        virtual std::string getSocketEndpoint();
        virtual int handle_read(const boost::system::error_code& error, size_t bytes_transferred);
        virtual int handle_write(const boost::system::error_code& error);
    private:
        // vars
        tcp::socket socket_;
        http::response<http::string_body> httpResponse_;
        RequestHandlerDelegate requestHandlerDelegate_;
        std::map<std::string, std::shared_ptr<RequestHandlerFactory>> factory_routes_;
        enum { max_length = 1024 };
        char data_[max_length];
};

#endif