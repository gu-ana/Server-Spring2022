#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <string>

#include <iostream>


class Header_Handler{
    /*
    Purpose is to handle and form server's HTTP response headers
    including HTTP version, type, and content-type headers.
    */
    public:
	std::string okHeader = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\n"; 
};
