#include <boost/bind.hpp>
#include <boost/asio.hpp>

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
        enum { max_length = 1024 };
        char data_[max_length];
};