#include "gtest/gtest.h"
#include <string>
#include "session.h"

class SessionTest : public ::testing::Test {
  protected:
    boost::asio::io_service io_service;
    Session session = Session(io_service);
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
