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

// test for bad request
TEST_F(RequestHandlerDelegateTest, BadRequest)
{
	char incorrect[] = "";
    delegate.processRequest(incorrect, httpResponse, ip);
	EXPECT_EQ(httpResponse.result_int(), 404);
	EXPECT_EQ(httpResponse.body(), "Bad Request\n");
}

// test for valid echo request
TEST_F(RequestHandlerDelegateTest, ValidEchoRequest)
{
	httpResponse = {};
	char correct[] = "GET /echo HTTP/1.1\r\n\r\n";
	delegate.processRequest(correct, httpResponse, ip);
	EXPECT_EQ(httpResponse.result_int(), 200);
}