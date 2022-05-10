#include <string>
#include <map>

#include "gtest/gtest.h"
#include "session.h"
#include "config_parser.h"
#include "request_handler_factory.h"

class SessionTest : public ::testing::Test
{
  protected:
    boost::asio::io_service io_service;
    std::map<std::string, std::shared_ptr<RequestHandlerFactory>> routes;
    Session session = Session(io_service, routes);
    boost::system::error_code ec;
};

