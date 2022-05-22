#include <string>
#include <map>

#include "gtest/gtest.h"
#include "session.h"
#include "config_parser.h"
#include "request_handler_factory/request_handler_factory.h"

class SessionTest : public ::testing::Test
{
	void SetUp()
	{
		session = new Session(io_service, routes);
	}

	protected:
		boost::asio::io_service io_service;
		std::map<std::string, std::shared_ptr<RequestHandlerFactory>> routes;
		Session* session;
		boost::system::error_code ec;
};

TEST_F(SessionTest, StartSuccess)
{
	session->start();
	delete session;
}

TEST_F(SessionTest, ReadFailure)
{
	ec = boost::system::errc::make_error_code(boost::system::errc::not_connected);
	bool retVal = session->handle_read(ec, 0);
	EXPECT_EQ(retVal, 1);
}

TEST_F(SessionTest, WriteSuccess)
{
	ec = boost::system::errc::make_error_code(boost::system::errc::success);
	bool retVal = session->handle_write(ec);
	EXPECT_EQ(retVal, 0);
	delete session;	
}

TEST_F(SessionTest, WriteFailure)
{
	ec = boost::system::errc::make_error_code(boost::system::errc::not_connected);
	bool retVal = session->handle_write(ec);
	EXPECT_EQ(retVal, 1);
}

