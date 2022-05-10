#ifndef SERVER_H
#define SERVER_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>


#include "config_parser.h"
#include "session.h"
#include "request_handler_factory.h"
using boost::asio::ip::tcp;



class Server {
    public:
        Server(boost::asio::io_service& io_service, short port, NginxConfig* config);
    private:
        //functions
        void start_accept();
        void handle_accept(Session* new_session, const boost::system::error_code& error);
        void create_factory_map();
        //variables
        boost::asio::io_service& io_service_;
        tcp::acceptor acceptor_;
        NginxConfig* config_;
        std::map<std::string, std::shared_ptr<RequestHandlerFactory>> routes;

        
};


#endif