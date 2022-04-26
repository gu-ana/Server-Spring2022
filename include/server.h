#ifndef SERVER_H
#define SERVER_H

#include <boost/bind.hpp>
#include <boost/asio.hpp>


#include "config_parser.h"
#include "session.h"
using boost::asio::ip::tcp;



class Server {
    public:
        Server(boost::asio::io_service& io_service, short port, NginxConfig* config);
    private:
        //functions
        void start_accept();
        void handle_accept(Session* new_session, const boost::system::error_code& error);
        //variables
        boost::asio::io_service& io_service_;
        tcp::acceptor acceptor_;
        NginxConfig* config_;

        
};


#endif