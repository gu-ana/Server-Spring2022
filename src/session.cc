#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <iostream>
#include <string>

namespace http = boost::beast::http;

#include "session.h"

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

http::response<http::string_body> Session::getHttpResponse()
{
  return httpResponse_;
}

void Session::handle_http(std::string target, char* data, RequestHandler* handler)
{
    if(target == "/echo/" || target == "/echo" ) 
    {
      handler = new EchoHandler;
      LOG(info) << "Echo request received from " << socket_.remote_endpoint() << "\n";
    }
    else if (target.find("/static") != std::string::npos )
    {
      handler = new StaticHandler;
      LOG(info) << "Static request received from " << socket_.remote_endpoint() << "\n";
      ((StaticHandler*) handler)->set_map(config_->getRoot());
    }

    if(handler == nullptr) {
      httpResponse_.version(11);
      httpResponse_.result(http::status::bad_request);
      httpResponse_.set(http::field::content_type, "text/plain");
      httpResponse_.body() = "Bad Request \n";
      httpResponse_.prepare_payload();
      LOG(info) << "HTTP response 400: Received bad request target \n";
    }
    else if(!handler->format_request(data)) 
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
}

//handle method is called after read is complete.
int Session::handle_read(const boost::system::error_code& error,
    size_t bytes_transferred)
{
  if (!error)
  {
    httpResponse_ = {};
    std::string target = get_target(data_);
    RequestHandler* handler = nullptr;
    handle_http(target, data_, handler);

    http::async_write(socket_, httpResponse_, boost::bind(&Session::handle_write, this,
                                boost::asio::placeholders::error));
    LOG(info) << "HTTP response sent to client at " << socket_.remote_endpoint()  << "\n";
    memset(data_, 0, 1024);
    if(handler != nullptr) {
      delete handler;
    }
    
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
