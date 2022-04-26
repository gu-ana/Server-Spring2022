#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <string>

namespace http = boost::beast::http;

#include "logger.h"
#include "request_handler.h"
#include "session.h"

Session::Session(boost::asio::io_service& io_service, NginxConfig* config): socket_(io_service), config_(config) {
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

std::string get_target(std::string httpRequestString) 
{
  http::request_parser<http::string_body> req_parser;
    req_parser.eager(true);

    boost::system::error_code ec;
    req_parser.put(boost::asio::buffer(httpRequestString), ec);
    if(ec) 
    {
        LOG(severity_level::error) << "Unable to parse request string error code:" << ec.message() << '\n' ; 
        return "";
    }
    return std::string(req_parser.get().target()).c_str();
}
//handle method is called after read is complete.
int Session::handle_read(const boost::system::error_code& error,
    size_t bytes_transferred)
{
  if (!error)
  { 

    std::string str(data_, bytes_transferred);
    httpResponse_ = {};
    std::string target = get_target(data_);
    RequestHandler* handler;
    if(target == "/echo/" || target == "/echo" ) 
    {
      handler = new EchoHandler;
      LOG(info) << "Echo request received \n";
    }
    else 
    {
      handler = new StaticHandler;
      LOG(info) << "Static request received \n";
      ((StaticHandler*) handler)->set_map(config_->getRoot());
    }

    if(!handler->format_request(data_)) 
    {
      LOG(severity_level::error) << "Unable to format request \n";
      httpResponse_.version(11);
      httpResponse_.result(http::status::ok);
      httpResponse_.set(http::field::content_type, "text/plain");
      httpResponse_.body() = "Bad Format \n";
      httpResponse_.prepare_payload();
    }
    else 
    {
      handler->handle_request(httpResponse_);
    }

    http::async_write(socket_, httpResponse_, boost::bind(&Session::handle_write, this,
                                boost::asio::placeholders::error));
    LOG(info) << "HTTP response sent to client \n";
    memset(data_, 0, 1024);
    delete handler;
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
