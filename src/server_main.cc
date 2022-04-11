//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//



#include <iostream>

#include "server.h"
#include "config_parser.h"

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 2)
    {
      std::cerr << "Usage: async_tcp_echo_server <config_file>\n";
      return 1;
    }
    NginxConfigParser parser;
    NginxConfig config;
    if (!parser.Parse(argv[1], &config)) {
      std::cerr << "Config file not able to be parsed.\n";
      return 1;
    }
    int port = config.getPort();
    if (port < 0) {
      std::cerr << "Unable to find valid Port from config file.\n";
      return 1;
    }

    boost::asio::io_service io_service;

    Server s(io_service, port);

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
