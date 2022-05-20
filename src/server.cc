#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "server.h"
#include "logger.h"
#include "session.h"
#include "request_handler_factory/request_handler_factory.h"
#include "request_handler_factory/static_handler_factory.h"
#include "request_handler_factory/echo_handler_factory.h"
#include "request_handler_factory/api_handler_factory.h"
#include "request_handler_factory/error_handler_factory.h"
#include "request_handler_factory/sleep_handler_factory.h"
#include "request_handler_factory/health_handler_factory.h"
#include "real_file_system.h"

using boost::asio::ip::tcp;

//server definitions
Server::Server(boost::asio::io_service& io_service, short port, NginxConfig* config, std::size_t thread_pool_size)
  : io_service_(io_service), acceptor_(io_service, tcp::endpoint(tcp::v4(), port)), config_(config), 
    thread_pool_size_(thread_pool_size)
{
  create_factory_map();  // create routes map (location, requesthandlerfactory_sharedptr)
  start_accept();
  LOG(info) << "Listening on port " << port;
}

void Server::run()
{
  // Create a pool of threads to run all of the io_services.
  std::vector<boost::shared_ptr<boost::thread> > threads;
  for (std::size_t i = 0; i < thread_pool_size_; ++i)
  {
    boost::shared_ptr<boost::thread> thread(new boost::thread(
          boost::bind(&boost::asio::io_service::run, &io_service_)));
    threads.push_back(thread);
  }

  // Wait for all threads in the pool to exit.
  for (std::size_t i = 0; i < threads.size(); ++i)
    threads[i]->join();
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
    // create sleep handler factory mapping
    std::shared_ptr<RequestHandlerFactory> sleep_factory(new SleepHandlerFactory());
    routes.insert({"/sleep", sleep_factory});
    // create health handler factory mapping
    std::shared_ptr<RequestHandlerFactory> health_factory(new HealthHandlerFactory());
    routes.insert({"/health", health_factory});
    // create 404 (error) handler factory
    std::shared_ptr<RequestHandlerFactory> error_factory(new ErrorHandlerFactory());
    routes.insert({"/", error_factory});

    // for all parsed static locations in config, add mapping to routes
    auto filesystem_map = config_->get_filesystem_map();
    std::map<std::string, std::string>::iterator fileMappingIter;
    std::map<std::string, std::string> static_handler_map = filesystem_map["StaticHandler"];
    std::map<std::string, std::string> api_handler_map = filesystem_map["ApiHandler"];
    for (fileMappingIter=static_handler_map.begin(); fileMappingIter!=static_handler_map.end(); fileMappingIter++)
    {
        std::string location = fileMappingIter->first;
        std::string root = fileMappingIter->second;
        std::shared_ptr<RequestHandlerFactory> static_factory(new StaticHandlerFactory(location,root));
        routes.insert({location, static_factory});
    }

    // init filesystem
    std::shared_ptr<FileSystem> fs(new RealFileSystem());
    for (fileMappingIter=api_handler_map.begin(); fileMappingIter!=api_handler_map.end(); fileMappingIter++)
    {
        std::string location = fileMappingIter->first;
        std::string data_path = fileMappingIter->second;
        std::shared_ptr<RequestHandlerFactory> api_factory(new ApiHandlerFactory(location, data_path, fs));
        routes.insert({location, api_factory});
    }

}