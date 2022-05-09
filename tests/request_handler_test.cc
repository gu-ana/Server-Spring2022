#include <map>
#include <string>

#include "gtest/gtest.h"
#include "request_handler.h"
#include "echo_handler.h"
#include "static_handler.h"
#include "error_handler.h"

class RequestHandlerTest : public ::testing::Test
{
	protected:
		http::response<http::string_body> httpResponse;
		http::request<http::string_body> httpRequest;
};

class EchoHandlerTest : public RequestHandlerTest
{
	protected:
		EchoHandler echo_handler;
};

class ErrorHandlerTest : public RequestHandlerTest
{
	protected:
		ErrorHandler error_handler;
};

class StaticHandlerTest : public RequestHandlerTest
{
	protected:
		StaticHandler static_handler = StaticHandler("/static/", "./files");

	void SetUp()
	{
		httpRequest = {};
	}
};

// helper function to set http::request objects
bool parse_request(std::string httpRequestString, http::request<http::string_body>& httpRequest) 
{
  http::request_parser<http::string_body> req_parser;
  req_parser.eager(true);

  boost::system::error_code ec;
  req_parser.put(boost::asio::buffer(httpRequestString), ec);
  if(ec) 
  {
      return false;
  }
  httpRequest = req_parser.get();
  return true;
}

// test correct echo
TEST_F(EchoHandlerTest, CorrectFormat)
{
	char correct_echo[] = "GET /echo HTTP/1.1\r\n\r\n";
	parse_request(correct_echo, httpRequest);
	bool retVal = echo_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);
	EXPECT_EQ(httpResponse.result_int(), 200);
	EXPECT_EQ(httpResponse.body(), correct_echo);
}

// test that ErrorHandler returns HTTP 404
TEST_F(ErrorHandlerTest, HTTP404Response)
{
	char invalid_request[] = "blah";
	parse_request(invalid_request, httpRequest);
	bool retVal = error_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);
	EXPECT_EQ(httpResponse.result_int(), 404);
	EXPECT_EQ(httpResponse.body(), "Bad Request\n");
}

// test exisitng txt file
TEST_F(StaticHandlerTest, ValidTxt)
{
	char correct_txt[] = "GET /static/file1.txt HTTP/1.1\r\n\r\n";
	static_handler = StaticHandler("/static/", "./files");
	parse_request(correct_txt, httpRequest);
	bool retVal = static_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);
	EXPECT_EQ(httpResponse.result_int(), 200);
}

// test existing png file
TEST_F(StaticHandlerTest, ValidPng)
{
	char correct_png[] = "GET /static/help/hutao.png HTTP/1.1\r\n\r\n";
	static_handler = StaticHandler("/static/help/", "./files/images");
	parse_request(correct_png, httpRequest);
	bool retVal = static_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);
	EXPECT_EQ(httpResponse.result_int(), 200);
}

// test non-existent file with valid format
TEST_F(StaticHandlerTest, FileNotFound)
{
	char does_not_exist[] = "GET /static/file2.txt HTTP/1.1\r\n\r\n";
	static_handler = StaticHandler("/static/help/", "./files/images");
	parse_request(does_not_exist, httpRequest);
	bool retVal = static_handler.handle_request(httpRequest, httpResponse);
	EXPECT_FALSE(retVal);
	EXPECT_EQ(httpResponse.result_int(), 404);
	EXPECT_EQ(httpResponse.body(), "File not found\n");
}



