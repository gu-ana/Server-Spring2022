#include <boost/bind.hpp>
#include <boost/asio.hpp>

#include "session.h"

Session::Session(boost::asio::io_service& io_service): socket_(io_service) {}

tcp::socket& Session::socket()
{
  return socket_;
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
    boost::asio::async_write(socket_,
                            boost::asio::buffer(data_, bytes_transferred),
                              boost::bind(&Session::handle_write, this,
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

