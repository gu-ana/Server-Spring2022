#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "session.h"
using boost::asio::ip::tcp;

class Server {
    public:
        Server(boost::asio::io_service& io_service, short port);
    private:
        //functions
        void start_accept();
        void handle_accept(Session* new_session, const boost::system::error_code& error);
        //variables
        boost::asio::io_service& io_service_;
        tcp::acceptor acceptor_;
};


