#include "gtest/gtest.h"
#include "request_handler_delegate.h"

class RequestHandlerDelegateTest : public ::testing::Test
{
  protected:
  	http::response<http::string_body> httpResponse;
    RequestHandlerDelegate delegate;
	std::map<std::string, std::string> map_;
	std::string ip = "127.1.0.0";

	void SetUp()
	{
		httpResponse = {};
		map_.insert({"/static/", "./files"});
		delegate.set_map(map_);
	}
};

// test for incorrectly formatted request
TEST_F(RequestHandlerDelegateTest, IncorrectFormat)
{
	char incorrect_format[] = "";
    delegate.processRequest(incorrect_format, httpResponse, ip);
	EXPECT_EQ(httpResponse.result_int(), 404);
	EXPECT_EQ(httpResponse.body(), "Bad Request\n");
}

// TODO @Will uncomment this
// // test for non-echo or non-static request
// TEST_F(RequestHandlerDelegateTest, BadRequest)
// {
// 	char not_echo_or_static[] = "GET /echmo HTTP/1.1\r\n\r\n";
//     delegate.processRequest(not_echo_or_static, httpResponse, ip);
// 	EXPECT_EQ(httpResponse.result_int(), 404);
// 	EXPECT_EQ(httpResponse.body(), "Bad Request\n");
// }

// test for valid echo request
TEST_F(RequestHandlerDelegateTest, ValidEchoRequest)
{
	char correct[] = "GET /echo HTTP/1.1\r\n\r\n";
	delegate.processRequest(correct, httpResponse, ip);
	EXPECT_EQ(httpResponse.result_int(), 200);
}