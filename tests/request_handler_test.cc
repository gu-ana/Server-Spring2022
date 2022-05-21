#include <map>
#include <string>
#include <iostream>

#include "gtest/gtest.h"
#include "request_handler/request_handler.h"
#include "request_handler/echo_handler.h"
#include "request_handler/static_handler.h"
#include "request_handler/error_handler.h"
#include "request_handler/bad_request_handler.h"
#include "request_handler/sleep_handler.h"
#include "request_handler/health_handler.h"
#include "request_handler/api_handler.h"
#include "fake_file_system.h"

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

class SleepHandlerTest : public RequestHandlerTest
{
	protected:
		SleepHandler sleep_handler;
};

class HealthHandlerTest : public RequestHandlerTest
{
	protected:
		HealthHandler health_handler;
};

class ErrorHandlerTest : public RequestHandlerTest
{
	protected:
		ErrorHandler error_handler;
};

class BadRequestHandlerTest : public RequestHandlerTest
{
	protected:
		BadRequestHandler bad_request_handler;
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
		std::shared_ptr<FileSystem> ffs;
		ApiHandler api_handler = ApiHandler("/api/", "./mnt/crud", ffs);
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

// test correct sleep
TEST_F(SleepHandlerTest, CorrectFormat)
{
	char correct_sleep[] = "GET /sleep HTTP/1.1\r\n\r\n";
	parse_request(correct_sleep, httpRequest);
	bool retVal = sleep_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);
	EXPECT_EQ(httpResponse.result_int(), 200);
}

// test that HealthHandler returns HTTP 200
TEST_F(HealthHandlerTest, CorrectFormat)
{
	char correct_health[] = "GET /health HTTP/1.1\r\n\r\n";
	parse_request(correct_health, httpRequest);
	bool retVal = health_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);
	EXPECT_EQ(httpResponse.result_int(), 200);
	EXPECT_EQ(httpResponse.body(), "OK\n");
}

// test that ErrorHandler returns HTTP 404
TEST_F(ErrorHandlerTest, HTTP404Response)
{
	char invalid_request[] = "GET / HTTP/1.1\r\n\r\n";
	parse_request(invalid_request, httpRequest);
	bool retVal = error_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);
	EXPECT_EQ(httpResponse.result_int(), 404);
	EXPECT_EQ(httpResponse.body(), "Could not handle request\n");
}

// test malformed request
TEST_F(BadRequestHandlerTest, HTTP400Response)
{
	char invalid_request[] = "";
	parse_request(invalid_request, httpRequest);
	bool retVal = bad_request_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);
	EXPECT_EQ(httpResponse.result_int(), 400);
	EXPECT_EQ(httpResponse.body(), "Bad Request\n");
}

// test existing txt file
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
	std::string content = "{\"data\": 1}";
	httpRequest.method(http::verb::post);
	httpRequest.target("/api/shoes");
	httpRequest.body() = content;
	std::shared_ptr<FileSystem> ffs(new FakeFileSystem());
	api_handler = ApiHandler("/api/", "./mnt/crud", ffs);
	bool retVal = api_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);
	EXPECT_EQ(httpResponse.body(), "{\"id\": 1}");
	retVal = api_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);
	EXPECT_EQ(httpResponse.body(), "{\"id\": 2}");
}

// POST creates a file in the data_path/entity with the correct ID
TEST_F(ApiHandlerTest, ApiPOSTCreateFile)
{
	std::string content = "{\"data\": 1}";
	httpRequest.method(http::verb::post);
	httpRequest.target("/api/shoes");
	httpRequest.body() = content;
	std::shared_ptr<FileSystem> ffs(new FakeFileSystem());
	api_handler = ApiHandler("/api/", "./mnt/crud", ffs);
	bool retVal = api_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);
	EXPECT_TRUE(ffs->exists("./mnt/crud/shoes/1"));
}

// GET reads file at data_path/entity/id and returns it
TEST_F(ApiHandlerTest, ApiGETReadFile)
{
	std::string content = "{\"data\": 1}";
	httpRequest.method(http::verb::post);
	httpRequest.target("/api/shoes");
	httpRequest.body() = content;
	std::shared_ptr<FileSystem> ffs(new FakeFileSystem());
	api_handler = ApiHandler("/api/", "./mnt/crud", ffs);
	bool retVal = api_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);

	char get_file[] = "GET /api/shoes/1 HTTP/1.1\r\n\r\n";
	parse_request(get_file, httpRequest);
	retVal = api_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);
	EXPECT_EQ(httpResponse.body(), "Read file successfully\n");
}

// GET reads file at data_path/entity/id but file not found
TEST_F(ApiHandlerTest, ApiGETFileNotFound)
{
	std::string content = "{\"data\": 1}";
	httpRequest.method(http::verb::post);
	httpRequest.target("/api/shoes");
	httpRequest.body() = content;
	std::shared_ptr<FileSystem> ffs(new FakeFileSystem());
	api_handler = ApiHandler("/api/", "./mnt/crud", ffs);
	bool retVal = api_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);

	char get_wrong_file[] = "GET /api/shoes/2 HTTP/1.1\r\n\r\n";
	parse_request(get_wrong_file, httpRequest);
	retVal = api_handler.handle_request(httpRequest, httpResponse);
	EXPECT_FALSE(retVal);
	EXPECT_EQ(httpResponse.body(), "Requested ID does not exist\n");
}

// PUT body was successfully written to new data_path/entity/id
TEST_F(ApiHandlerTest, ApiPUTNewFile)
{
	std::string content = "{\"data\": 1}";
	httpRequest.method(http::verb::put);
	httpRequest.target("/api/shoes/1");
	httpRequest.body() = content;
	std::shared_ptr<FileSystem> ffs(new FakeFileSystem());
	api_handler = ApiHandler("/api/", "./mnt/crud", ffs);
	bool retVal = api_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);
	EXPECT_TRUE(ffs->exists("./mnt/crud/shoes/1"));
}

// PUT body was successfully written to update data_path/entity/id
TEST_F(ApiHandlerTest, ApiPUTUpdateFile)
{
	std::string content = "{\"data\": 1}";
	httpRequest.method(http::verb::post);
	httpRequest.target("/api/shoes");
	httpRequest.body() = content;
	std::shared_ptr<FileSystem> ffs(new FakeFileSystem());
	api_handler = ApiHandler("/api/", "./mnt/crud", ffs);
	bool retVal = api_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);

	std::string new_content = "{\"data\": 2}";
	httpRequest.method(http::verb::put);
	httpRequest.target("/api/shoes/1");
	httpRequest.body() = content;
	retVal = api_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);
	EXPECT_TRUE(ffs->exists("./mnt/crud/shoes/1"));
}

// DELETE removes the file at data_path/entity/id
TEST_F(ApiHandlerTest, ApiDELETERemoveFile)
{
	std::string content = "{\"data\": 1}";
	httpRequest.method(http::verb::post);
	httpRequest.target("/api/shoes");
	httpRequest.body() = content;
	std::shared_ptr<FileSystem> ffs(new FakeFileSystem());
	api_handler = ApiHandler("/api/", "./mnt/crud", ffs);
	bool retVal = api_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);

	httpRequest.method(http::verb::delete_);
	httpRequest.target("/api/shoes/1");
	retVal = api_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);
	EXPECT_FALSE(ffs->exists("./mnt/crud/shoes/1"));
	EXPECT_EQ(httpResponse.body(), "Successfully deleted ID\n");
}

// DELETE returns an error if the file at data_path/entity/id does not exist
TEST_F(ApiHandlerTest, ApiDELETEFileNotFound)
{
	std::string content = "{\"data\": 1}";
	httpRequest.method(http::verb::post);
	httpRequest.target("/api/shoes");
	httpRequest.body() = content;
	std::shared_ptr<FileSystem> ffs(new FakeFileSystem());
	api_handler = ApiHandler("/api/", "./mnt/crud", ffs);
	bool retVal = api_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);

	httpRequest.method(http::verb::delete_);
	httpRequest.target("/api/shoes/2");
	retVal = api_handler.handle_request(httpRequest, httpResponse);
	EXPECT_FALSE(retVal);
	EXPECT_TRUE(ffs->exists("./mnt/crud/shoes/1"));
	EXPECT_EQ(httpResponse.body(), "Could not delete requested ID\n");
}

TEST_F(ApiHandlerTest, ApiGETList)
{
	std::string content = "{\"data\": 1}";
	httpRequest.method(http::verb::post);
	httpRequest.target("/api/shoes");
	httpRequest.body() = content;
	std::shared_ptr<FileSystem> ffs(new FakeFileSystem());
	api_handler = ApiHandler("/api/", "./mnt/crud", ffs);
	bool retVal = api_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);
	retVal = api_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);

	char get_list[] = "GET /api/shoes HTTP/1.1\r\n\r\n";
	parse_request(get_list, httpRequest);
	retVal = api_handler.handle_request(httpRequest, httpResponse);
	EXPECT_TRUE(retVal);
	EXPECT_EQ(httpResponse.body(), "[1, 2]");
}

// List: GET /api/entity for invalid entity
TEST_F(ApiHandlerTest, ApiGETEntityNotFound)
{
	httpRequest.method(http::verb::get);
	httpRequest.target("/api/shoes");
	std::shared_ptr<FileSystem> ffs(new FakeFileSystem());
	api_handler = ApiHandler("/api/", "./mnt/crud", ffs);
	bool retVal = api_handler.handle_request(httpRequest, httpResponse);
	EXPECT_FALSE(retVal);
	EXPECT_EQ(httpResponse.body(), "Invalid entity requested\n");
}