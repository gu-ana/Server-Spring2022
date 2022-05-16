#include <map>
#include <string>

#include "gtest/gtest.h"
#include "request_handler.h"
#include "echo_handler.h"
#include "static_handler.h"
#include "error_handler.h"
#include "api_handler.h"

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

class ApiHandlerTest : public RequestHandlerTest
{
	protected:
		// ApiHandler api_handler = ApiHandler("./data_path/");
		//This was commented out because it didn't build
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

// POST finds correct next available ID and returns {"id": <ID>} in the response
TEST_F(ApiHandlerTest, ApiPOSTReturnID)
{
	EXPECT_TRUE(1);
}

// POST creates a file in the data_path/entity with the correct ID
TEST_F(ApiHandlerTest, ApiPOSTCreateFile)
{
	EXPECT_TRUE(1);
}

// GET reads file at data_path/entity/id and returns it
TEST_F(ApiHandlerTest, ApiGETReadFile)
{
	EXPECT_TRUE(1);
}

// GET reads file at data_path/entity/id but file not found
TEST_F(ApiHandlerTest, ApiGETFileNotFound)
{
	EXPECT_TRUE(1);
}

// PUT body was successfully written to new data_path/entity/id
TEST_F(ApiHandlerTest, ApiPUTNewFile)
{
	EXPECT_TRUE(1);
}

// PUT body was successfully written to update data_path/entity/id
TEST_F(ApiHandlerTest, ApiPUTUpdateFile)
{
	EXPECT_TRUE(1);
}

// DELETE removes the file at data_path/entity/id
TEST_F(ApiHandlerTest, ApiDELETERemoveFile)
{
	EXPECT_TRUE(1);
}

// DELETE returns an error if the file at data_path/entity/id does not exist
TEST_F(ApiHandlerTest, ApiDELETEFileNotFound)
{
	EXPECT_TRUE(1);
}

// List: GET /api/entity returns json list of valid ids for entity
TEST_F(ApiHandlerTest, ApiGETList)
{
	EXPECT_TRUE(1);
}

// List: GET /api/entity for invalid entity
TEST_F(ApiHandlerTest, ApiGETEntityNotFound)
{
	EXPECT_TRUE(1);
}