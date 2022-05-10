#include <boost/bind.hpp>
#include <boost/asio.hpp>


#include "server.h"
#include "logger.h"
#include "session.h"
#include "request_handler_factory.h"
#include "static_handler_factory.h"
#include "echo_handler_factory.h"
#include "error_handler_factory.h"

using boost::asio::ip::tcp;

//server definitions
Server::Server(boost::asio::io_service& io_service, short port, NginxConfig* config )
  : io_service_(io_service), acceptor_(io_service, tcp::endpoint(tcp::v4(), port)), config_(config)
{
  create_factory_map();  // create routes map (location, requesthandlerfactory_sharedptr)
  start_accept();
  LOG(info) << "Listening on port " << port;
}

void Server::start_accept()
{
  //every accept we create a new session object and pass in our created routes map
  Session* new_session = new Session(io_service_, routes);
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

/*
 * this method fills the 'routes' map with 'location' path to factory pairs.
*/
void Server::create_factory_map()
{
    // create echo handler factory mapping
    std::shared_ptr<RequestHandlerFactory> echo_factory(new EchoHandlerFactory());
    routes.insert({"/echo", echo_factory});
    // create 404 (error) handler factory
    std::shared_ptr<RequestHandlerFactory> error_factory(new ErrorHandlerFactory());
    routes.insert({"/", error_factory});

    // for all parsed static locations in config, add mapping to routes
    std::map<std::string, std::string> filesystem_map = config_->get_filesystem_map();
    std::map<std::string, std::string>::iterator fileMappingIter;
    for (fileMappingIter = filesystem_map.begin(); fileMappingIter != filesystem_map.end(); fileMappingIter++)
    {
        std::string location = fileMappingIter->first;
        std::string root = fileMappingIter->second;
        std::shared_ptr<RequestHandlerFactory> static_factory(new StaticHandlerFactory(location,root));
        routes.insert({location, static_factory});
    }
}