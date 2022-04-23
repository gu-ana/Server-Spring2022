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
#include <csignal>

#include "server.h"
#include "config_parser.h"
#include "logger.h"

void signalHandler(int signum)
{
  if (signum == SIGTERM)
  {
    LOG(info) << "Server was aborted...shutting down\n\n";
  }
  else
  {
    LOG(fatal) << "Server was interrupted...shutting down\n\n";
  }

   // any future clean up logic goes here

   exit(signum);
}

int main(int argc, char* argv[])
{
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);

  try
  {
    Logger::initLogger();

    if (argc != 2)
    {
      LOG(error) << "Usage: async_tcp_echo_server <config_file>\n";
      return 1;
    }
    NginxConfigParser parser;
    NginxConfig config;
    LOG(info) << "Parsing server config...";
    if (!parser.Parse(argv[1], &config)) {
      LOG(error) << "Config file not able to be parsed.\n";
      return 1;
    }
    int port = config.getPort();
    if (port < 0) {
      LOG(error) << "Unable to find valid Port from config file.\n";
      return 1;
    }

    boost::asio::io_service io_service;

    Server s(io_service, port);

    io_service.run();
  }
  catch (std::exception& e)
  {
    LOG(fatal) << "Exception: " << e.what() << "\n";
  }

  return 0;
}
