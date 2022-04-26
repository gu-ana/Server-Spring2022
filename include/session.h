#ifndef SESSION_H
#define SESSION_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>


#include "config_parser.h"
using boost::asio::ip::tcp;
namespace http = boost::beast::http;


class Session {
    public:
        //functions
        Session(boost::asio::io_service& io_service, NginxConfig* config);
        void start();
        tcp::socket& socket();
    
        //functions
        int handle_read(const boost::system::error_code& error, size_t bytes_transferred);
        int handle_write(const boost::system::error_code& error);
    private:
        //vars
        tcp::socket socket_;
        http::response<http::string_body> httpResponse_;
        NginxConfig* config_;
        enum { max_length = 1024 };
        char data_[max_length];
};

#endif