#include <string>
#include <map>

#include "gtest/gtest.h"
#include "session.h"
#include "config_parser.h"

class SessionTest : public ::testing::Test
{
  protected:
    boost::asio::io_service io_service;
    NginxConfig* config_ptr;
    Session session = Session(io_service, config_ptr);
    boost::system::error_code ec;
};

