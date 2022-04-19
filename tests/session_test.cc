#include "gtest/gtest.h"
#include <string>
#include "session.h"

class SessionTest : public ::testing::Test {
  protected:
    boost::asio::io_service io_service;
    Session session = Session(io_service);
    boost::system::error_code ec;
};

TEST_F(SessionTest, httpBody) {
  char test_data[] = "http test body data.\n";
  int success = session.handle_http(test_data, 21);
  EXPECT_TRUE(success);
  EXPECT_EQ(session.gethttpResponse().body(), std::string(test_data, 21));
}

TEST_F(SessionTest, statusCode) {
  char test_data[] = "http test body data.\n";
  int success = session.handle_http(test_data, 21);
  EXPECT_TRUE(success);
  EXPECT_EQ(session.gethttpResponse().result_int(), 200);
}

TEST_F(SessionTest, multipleRequests) {
  char test_data[] = "http test body data.\n";
  char test_data2[] = "another http body data test.\n";
  int success = session.handle_http(test_data, 21);
  EXPECT_TRUE(success);
  EXPECT_EQ(session.gethttpResponse().body(), std::string(test_data, 21));
  EXPECT_EQ(session.gethttpResponse().result_int(), 200);
  success = session.handle_http(test_data2, 29);
  EXPECT_TRUE(success);
  EXPECT_EQ(session.gethttpResponse().body(), std::string(test_data2, 29));
  EXPECT_EQ(session.gethttpResponse().result_int(), 200);
}

/*
TEST_F(SessionTest, getSocket) {
  tcp::socket session->socket();
  EXPECT_SUCCESS(socket);
}
*/

// test handle_write()'s error code handling
TEST_F(SessionTest, handle_write_errc) {
  ec = boost::system::errc::make_error_code(boost::system::errc::success);
  int retVal = session.handle_write(ec);
  EXPECT_EQ(retVal, 0);
}

/*
TEST_F(SessionTest, handle_write_errc2) {
  ec = boost::system::errc::make_error_code(boost::system::errc::not_supported);
  int retVal = session.handle_write(ec);
  EXPECT_EQ(retVal, -1);
}
*/

// same for Session's handle_read() method
TEST_F(SessionTest, handle_read_errc) {
  ec = boost::system::errc::make_error_code(boost::system::errc::success);
  int retVal = session.handle_read(ec, 1);
  EXPECT_EQ(retVal, 0);
}

/*
TEST_F(SessionTest, handle_read_errc2) {
  ec = boost::system::errc::make_error_code(boost::system::errc::not_supported);
  int retVal = session.handle_read(ec, 1);
  EXPECT_EQ(retVal, -1);
}
*/

