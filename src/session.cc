#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <string>

namespace http = boost::beast::http;


#include "session.h"

Session::Session(boost::asio::io_service& io_service): socket_(io_service) {
  // setting http response
  http::response<http::string_body> httpResponse_;
  httpResponse_.version(11);
}

tcp::socket& Session::socket()
{
  return socket_;
}

int Session::handle_http(char * data, size_t bytes_transferred) 
{
  // code source: https://www.boost.org/doc/libs/develop/libs/beast/doc/html/beast/using_http/message_containers.html
  // setting response header
  httpResponse_.result(http::status::ok);
  httpResponse_.set(http::field::content_type, "text/plain");
    
  // converts data to C++ string
  // sets body of response to data
  std::string str(data, bytes_transferred);
  httpResponse_.body() = str;

  // sets length of data
  httpResponse_.prepare_payload();
  return 1;
}

http::response<http::string_body> Session::gethttpResponse() {
  return httpResponse_;
}

void Session::start()
{
  socket_.async_read_some(boost::asio::buffer(data_, max_length),
      boost::bind(&Session::handle_read, this,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
}

//handle method is called after read is complete.
void Session::handle_read(const boost::system::error_code& error,
    size_t bytes_transferred)
{
  if (!error)
  { 
    /*
    // code source: https://www.boost.org/doc/libs/develop/libs/beast/doc/html/beast/using_http/message_containers.html
    // setting response header
    httpResponse_.result(http::status::ok);
    httpResponse_.set(http::field::content_type, "text/plain");
    
    // converts data to C++ string
    // sets body of response to data
    std::string str(data_, bytes_transferred);
    httpResponse_.body() = str;

    // sets length of data
    httpResponse_.prepare_payload();
    */
    handle_http(data_, bytes_transferred);
    http::async_write(socket_, httpResponse_, boost::bind(&Session::handle_write, this,
                               boost::asio::placeholders::error));
  }
  else
  {
    delete this;
  }
}

//similarly, handle_write is called after a write has completed.
void Session::handle_write(const boost::system::error_code& error)
{
  if (!error)
  {
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
                              boost::bind(&Session::handle_read, this,
                              boost::asio::placeholders::error,
                              boost::asio::placeholders::bytes_transferred));
  }
  else
  {
    delete this;
  }
}
