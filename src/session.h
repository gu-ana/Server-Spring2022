#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "header_handler.h"

using boost::asio::ip::tcp;

class Session {
    public:
        //functions
        Session(boost::asio::io_service& io_service);
        void start();
        tcp::socket& socket();
    private:
        //functions
        void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
        void handle_write(const boost::system::error_code& error);
        //vars
        tcp::socket socket_;
	Header_Handler head_handler;
        enum { max_length = 1024 };
        char data_[max_length];
};
