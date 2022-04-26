#include <boost/bind.hpp>
#include <boost/asio.hpp>


#include "server.h"
#include "logger.h"

using boost::asio::ip::tcp;

//server definitions
Server::Server(boost::asio::io_service& io_service, short port, NginxConfig* config )
  : io_service_(io_service), acceptor_(io_service, tcp::endpoint(tcp::v4(), port)), config_(config)
{
  start_accept();
  LOG(info) << "Listening on port " << port;
}

void Server::start_accept()
{
  //every accept we create a new session object and pass in our parsed config file
  Session* new_session = new Session(io_service_, config_);
  acceptor_.async_accept(new_session->socket(),
                         boost::bind(&Server::handle_accept, this, new_session,
                         boost::asio::placeholders::error));
}

void Server::handle_accept(Session* new_session, const boost::system::error_code& error)
{
  if (!error)
  {
    new_session->start();
  }
  else
  {
    delete new_session;
  }

  start_accept();
}

