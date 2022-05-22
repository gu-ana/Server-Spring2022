#include <string>
#include <sstream>

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "server.h"
#include "session.h"
#include "config_parser.h"
#include "request_handler_factory/request_handler_factory.h"

class ServerTest : public ::testing::Test
{
	void SetUp()
	{
		NginxConfigParser parser;
		std::stringstream ss;
		ss << "server { listen 8000; location /static/ StaticHandler {root ./files;} location /api/ ApiHandler {data_path ./mnt;} }";
		parser.Parse(&ss, &config);
		config.extract_filesystem_map();
		port = 8000;
		thread_pool_size = 0;
	}

	protected:
		boost::asio::io_service io_service;
		short port;
		NginxConfig config;
		std::size_t thread_pool_size;
};

TEST_F(ServerTest, CreateServer)
{
	Server s(io_service, port, &config, thread_pool_size);
	auto filesystem_map = config.get_filesystem_map();
	EXPECT_THAT(filesystem_map["StaticHandler"], ::testing::SizeIs(1));
	EXPECT_THAT(filesystem_map["ApiHandler"], ::testing::SizeIs(1));
}

TEST_F(ServerTest, RunServer)
{
	Server s(io_service, port, &config, thread_pool_size);
	s.run();
}