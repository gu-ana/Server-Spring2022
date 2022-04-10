#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <string>


#include "session.h"

Session::Session(boost::asio::io_service& io_service): socket_(io_service) {
  // Header_Handler head_handler;
}

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
    Header_Handler head_handler;
    // vector of const_buffers, use to combine header and data buffers.
    std::vector<boost::asio::const_buffer> buffers;
    //use the header_handler class to get the OK header and length
    buffers.push_back(boost::asio::buffer(head_handler.okHeader, head_handler.okHeader.length()));
    //must also push back using bytes_transferred or else the buffer will increase in size
    buffers.push_back(boost::asio::buffer(data_, bytes_transferred));

    // then echo back the formed response to the client...
    boost::asio::async_write(socket_, buffers,
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

