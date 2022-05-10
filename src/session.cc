#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <string>

namespace http = boost::beast::http;

#include "session.h"
#include "request_handler_factory.h"

Session::Session(boost::asio::io_service& io_service, std::map<std::string, std::shared_ptr<RequestHandlerFactory>> factory_routes): socket_(io_service), factory_routes_(factory_routes) 
{
  requestHandlerDelegate_.set_factory_routes(factory_routes_);
}

void Session::start()
{
  socket_.async_read_some(boost::asio::buffer(data_, max_length),
      boost::bind(&Session::handle_read, this,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
}

std::string Session::getSocketEndpoint()
{
    std::stringstream ss;
    ss << socket_.remote_endpoint();
    return ss.str();
}

tcp::socket& Session::socket()
{
  return socket_;
}

//handle method is called after read is complete.
int Session::handle_read(const boost::system::error_code& error,
    size_t bytes_transferred)
{
  if (!error)
  {
    httpResponse_ = {};
    requestHandlerDelegate_.processRequest(data_, httpResponse_, getSocketEndpoint());

    http::async_write(socket_, httpResponse_, boost::bind(&Session::handle_write, this,
                                boost::asio::placeholders::error));
    LOG(info) << "HTTP response sent to client at " << socket_.remote_endpoint()  << "\n";
  }
  else
  {
    delete this;
  }
  return 0;
}

//similarly, handle_write is called after a write has completed.
int Session::handle_write(const boost::system::error_code& error)
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
  return 0;
}
