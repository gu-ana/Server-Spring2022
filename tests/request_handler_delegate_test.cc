#include "gtest/gtest.h"
#include "request_handler_delegate.h"
#include "request_handler_factory/request_handler_factory.h"
#include "request_handler_factory/static_handler_factory.h"
#include "request_handler_factory/echo_handler_factory.h"
#include "request_handler_factory/error_handler_factory.h"
#include "request_handler_factory/health_handler_factory.h"
#include "request_handler_factory/bad_request_handler_factory.h"

class RequestHandlerDelegateTest : public ::testing::Test
{
  protected:
  	http::response<http::string_body> httpResponse;
    RequestHandlerDelegate delegate;
	std::map<std::string, std::shared_ptr<RequestHandlerFactory>> factory_routes_;
	std::string ip = "127.1.0.0";

	void SetUp()
	{
		httpResponse = {};
        std::string location = "/static/";
        std::string root_path = "./files";
        std::shared_ptr<RequestHandlerFactory> static_factory(new StaticHandlerFactory(location, root_path));
		std::shared_ptr<RequestHandlerFactory> echo_factory(new EchoHandlerFactory());
        std::shared_ptr<RequestHandlerFactory> error_factory(new ErrorHandlerFactory());
        std::shared_ptr<RequestHandlerFactory> health_factory(new HealthHandlerFactory());
        std::shared_ptr<RequestHandlerFactory> bad_request_factory(new BadRequestHandlerFactory());
        factory_routes_.insert({location, static_factory}); // static mapping
        factory_routes_.insert({"/echo", echo_factory});
        factory_routes_.insert({"/", error_factory});
        factory_routes_.insert({"/health", health_factory});
        factory_routes_.insert({"bad request", bad_request_factory});
		delegate.set_factory_routes(factory_routes_);
	}
};

// test for incorrectly formatted request
TEST_F(RequestHandlerDelegateTest, IncorrectFormat)
{
	char incorrect_format[] = "";
    delegate.processRequest(incorrect_format, httpResponse, ip);
	EXPECT_EQ(httpResponse.result_int(), 400);
	EXPECT_EQ(httpResponse.body(), "Bad Request\n");
}

// test for request that does not reach one of configured handlers
TEST_F(RequestHandlerDelegateTest, UnconfiguredRequest)
{
	char unconfigured_request[] = "GET /echmo HTTP/1.1\r\n\r\n";
    delegate.processRequest(unconfigured_request, httpResponse, ip);
	EXPECT_EQ(httpResponse.result_int(), 404);
	EXPECT_EQ(httpResponse.body(), "Could not handle request\n");
}

// test for valid echo request
TEST_F(RequestHandlerDelegateTest, ValidEchoRequest)
{
	char correct[] = "GET /echo HTTP/1.1\r\n\r\n";
	delegate.processRequest(correct, httpResponse, ip);
	EXPECT_EQ(httpResponse.result_int(), 200);
}

// test for valid static request
TEST_F(RequestHandlerDelegateTest, ValidStaticRequest)
{
	char correct[] = "GET /static/file1.txt HTTP/1.1\r\n\r\n";
	delegate.processRequest(correct, httpResponse, ip);
	EXPECT_EQ(httpResponse.result_int(), 200);
}

// test for valid health request
TEST_F(RequestHandlerDelegateTest, ValidHealthRequest)
{
	char correct[] = "GET /health HTTP/1.1\r\n\r\n";
	delegate.processRequest(correct, httpResponse, ip);
	EXPECT_EQ(httpResponse.result_int(), 200);
}