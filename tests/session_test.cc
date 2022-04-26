#include <string>
#include <map>

#include "gtest/gtest.h"
#include "session.h"
#include "config_parser.h"

class SessionTest : public ::testing::Test {
  protected:
    boost::asio::io_service io_service;
    NginxConfig* config_ptr;
    Session session = Session(io_service, config_ptr);
    char data[1] = "";
    char invalid[16] = "invalid request";
    boost::system::error_code ec;
};

// test handle_write()'s error code handling
TEST_F(SessionTest, handle_write_errc) {
	ec = boost::system::errc::make_error_code(boost::system::errc::success);
	int retVal = session.handle_write(ec);
	EXPECT_EQ(retVal, 0);
}

// test for get_target() which identifies if request is echo, static, or bad request
TEST_F(SessionTest, ParseRequests) {
	std::string result = get_target("GET /echo HTTP/1.1\r\n\r\n");
	EXPECT_EQ(result, "/echo");

	result = get_target("blah");
	EXPECT_EQ(result, "");
}

// test for bad request
TEST_F(SessionTest, BadRequest) {
	session.handle_http("blah", data, nullptr);
	EXPECT_EQ(session.getHttpResponse().result_int(), 400);
	EXPECT_EQ(session.getHttpResponse().body(), "Bad Request \n");
}

// test for badly formatted static request
TEST_F(SessionTest, StaticBadFormatRequest) {
	RequestHandler* handler = new StaticHandler;
	session.handle_http("", invalid, handler);
	EXPECT_EQ(session.getHttpResponse().result_int(), 200);
	EXPECT_EQ(session.getHttpResponse().body(), "Bad Format \n");
	delete handler;
}

