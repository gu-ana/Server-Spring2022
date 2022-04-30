#include <map>
#include <string>

#include "gtest/gtest.h"
#include "request_handler.h"

class RequestHandlerTest : public ::testing::Test
{
  protected:
  	char wrong[5] = "blah";
  	http::response<http::string_body> httpResponse;
};

class EchoHandlerTest : public RequestHandlerTest
{
	protected:
		EchoHandler echo_handler;
};

class StaticHandlerTest : public RequestHandlerTest
{
	protected:
		StaticHandler static_handler;
		std::map<std::string, std::string> map_;
		char correct[39] = "GET /static/file1.txt HTTP/1.1\r\n\r\n";
		char correct_png[44] = "GET /static/help/hutao.png HTTP/1.1\r\n\r\n";
		char does_not_exist[39] = "GET /static/file2.txt HTTP/1.1\r\n\r\n";
};

// test correct echo
TEST_F(EchoHandlerTest, CorrectFormat)
{
	char correct[] = "GET /echo HTTP/1.1\r\n\r\n";
	bool success = echo_handler.format_request(correct);
	EXPECT_TRUE(success);

	int retVal = echo_handler.handle_request(httpResponse);
	EXPECT_EQ(retVal, 0);
	EXPECT_EQ(httpResponse.result_int(), 200);
	EXPECT_EQ(httpResponse.body(), correct);
}

// test bad echo
TEST_F(EchoHandlerTest, BadFormat)
{
	bool fail = echo_handler.format_request(wrong);
	EXPECT_FALSE(fail);
}

// test correct static
TEST_F(StaticHandlerTest, CorrectFormat)
{
	bool success = static_handler.format_request(correct);
	EXPECT_TRUE(success);
}

// test bad static
TEST_F(StaticHandlerTest, BadFormat)
{
	bool fail = static_handler.format_request(wrong);
	EXPECT_FALSE(fail);
}

// test longest prefix matching
TEST_F(StaticHandlerTest, PrefixMatching)
{
	map_.insert({"/static/", "/files"});
	std::string target = static_handler.longest_prefix_match(map_, "/static/help/hutao.jpg");
	EXPECT_EQ(target, "/files/help/hutao.jpg");

	map_.insert({"/static/help/", "/files/images"});
	target = static_handler.longest_prefix_match(map_, "/static/help/hutao.jpg");
	EXPECT_EQ(target, "/files/images/hutao.jpg");
}

// test existing txt file
TEST_F(StaticHandlerTest, ValidTxt)
{
	map_.insert({"/static/", "/files"});
	static_handler.set_map(map_);

	static_handler.format_request(correct);
	int retVal = static_handler.handle_request(httpResponse);
	EXPECT_EQ(retVal, 0);
	EXPECT_EQ(httpResponse.result_int(), 200);
}

// test existing png file
TEST_F(StaticHandlerTest, ValidPng)
{
	map_.insert({"/static/help/", "/files/images"});
	static_handler.set_map(map_);

	static_handler.format_request(correct_png);
	int retVal = static_handler.handle_request(httpResponse);
	EXPECT_EQ(retVal, 0);
	EXPECT_EQ(httpResponse.result_int(), 200);
}

// test non-existent file with valid format
TEST_F(StaticHandlerTest, FileNotFound)
{
	map_.insert({"/static/help/", "/files/images"});
	static_handler.set_map(map_);

	static_handler.format_request(does_not_exist);
	int retVal = static_handler.handle_request(httpResponse);
	EXPECT_EQ(retVal, 2);
	EXPECT_EQ(httpResponse.result_int(), 404);
	EXPECT_EQ(httpResponse.body(), "File not found");
}

// works locally but not on gcloud: test existing file with no permission
// must create a file with no read permissions for this to work
// TEST_F(StaticHandlerTest, FileNoPermissions) {
// 	map_.insert({"/static/", "/files"});
// 	static_handler.set_map(map_);

// 	static_handler.format_request("GET /static/no_permission.txt HTTP/1.1\r\n\r\n");
// 	int retVal = static_handler.handle_request(httpResponse);
// 	EXPECT_EQ(retVal, 2);
// 	EXPECT_EQ(httpResponse.result_int(), 404);
// 	EXPECT_EQ(httpResponse.body(), "Cannot Open File");
// }

