#include <map>
#include <string>

#include "gtest/gtest.h"
#include "request_handler.h"
#include "echo_handler.h"
#include "static_handler.h"

class RequestHandlerTest : public ::testing::Test
{
	protected:
		char wrong[5] = "blah";
		http::response<http::string_body> httpResponse;
		http::request<http::string_body> httpRequest;
};

class EchoHandlerTest : public RequestHandlerTest
{
	protected:
		EchoHandler echo_handler;
};

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

class StaticHandlerTest : public RequestHandlerTest
{
	protected:
		StaticHandler static_handler = StaticHandler("/static/", "./files");
		char correct_txt[39] = "GET /static/file1.txt HTTP/1.1\r\n\r\n";
		char correct_png[44] = "GET /static/help/hutao.png HTTP/1.1\r\n\r\n";
		char does_not_exist[39] = "GET /static/file2.txt HTTP/1.1\r\n\r\n";
};

// test correct echo
TEST_F(EchoHandlerTest, CorrectFormat)
{
	char correct[] = "GET /echo HTTP/1.1\r\n\r\n";
	parse_request(correct, httpRequest);

	bool retVal = echo_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);
	EXPECT_EQ(httpResponse.result_int(), 200);
	EXPECT_EQ(httpResponse.body(), correct);
}

// test exisitng txt file
TEST_F(StaticHandlerTest, ValidTxt)
{
	static_handler = StaticHandler("/static/", "./files");
	httpRequest = {};
	parse_request(correct_txt, httpRequest);
	bool retVal = static_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);
	EXPECT_EQ(httpResponse.result_int(), 200);
}

// test existing png file
TEST_F(StaticHandlerTest, ValidPng)
{
	static_handler = StaticHandler("/static/help/", "./files/images");
	httpRequest = {};
	parse_request(correct_png, httpRequest);
	bool retVal = static_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);
	EXPECT_EQ(httpResponse.result_int(), 200);
}

// test non-existent file with valid format
TEST_F(StaticHandlerTest, FileNotFound)
{
	static_handler = StaticHandler("/static/help/", "./files/images");
	httpRequest = {};
	parse_request(does_not_exist, httpRequest);
	bool retVal = static_handler.handle_request(httpRequest, httpResponse);
	EXPECT_FALSE(retVal);
	EXPECT_EQ(httpResponse.result_int(), 404);
	EXPECT_EQ(httpResponse.body(), "File not found\n");
}



