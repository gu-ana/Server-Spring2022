#include <string>
#include <map>

#include "gtest/gtest.h"
#include "config_parser.h"

class NginxConfigParserTest : public ::testing::Test
{
  protected:
    NginxConfigParser parser;
    NginxConfig out_config;	
};


// SimpleConfig testcase
TEST_F(NginxConfigParserTest, SimpleConfig)
{
  bool success = parser.Parse("./parser_tests/example_config", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, TokensConfig)
{
  bool success = parser.Parse("./parser_tests/tokens_config", &out_config);
  EXPECT_TRUE(success);
}

// Syntax checks
TEST_F(NginxConfigParserTest, BadSyntax1)
{
  bool success = parser.Parse("./parser_tests/syntax1_config", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, BadSyntax2)
{
  bool success = parser.Parse("./parser_tests/syntax2_config", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, BadSyntax3)
{
  bool success = parser.Parse("./parser_tests/syntax3_config", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, BadSyntax4)
{
  bool success = parser.Parse("./parser_tests/syntax4_config", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, BadSyntax5)
{
  bool success = parser.Parse("./parser_tests/syntax5_config", &out_config);
  EXPECT_FALSE(success);
}

// Empty config file should return true
TEST_F(NginxConfigParserTest, EmptyConfig)
{
  bool success = parser.Parse("./parser_tests/empty_config", &out_config);
  EXPECT_TRUE(success);
}

// Empty block should work with grammar
TEST_F(NginxConfigParserTest, EmptyBlock)
{
  bool success = parser.Parse("./parser_tests/emptyblock_config", &out_config);
  EXPECT_TRUE(success);
}


TEST_F(NginxConfigParserTest, Whitespace)
{
  bool success = parser.Parse("./parser_tests/whitespace_config", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, singleEscape)
{
  bool success = parser.Parse("./parser_tests/singleEscape_config", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, doubleEscape)
{
  bool success = parser.Parse("./parser_tests/doubleEscape_config", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, ConfigWithComment)
{
    bool success = parser.Parse("./parser_tests/comment_config", &out_config);
    EXPECT_TRUE(success);
}

// Testing port number test cases
TEST_F(NginxConfigParserTest, negativePort)
{
  bool success = parser.Parse("./parser_tests/negativePort_config", &out_config);
  EXPECT_TRUE(success);
  int port = out_config.getPort();
  EXPECT_EQ(port, -1);
}

TEST_F(NginxConfigParserTest, reservedPort)
{
  bool success = parser.Parse("./parser_tests/reservedPort_config", &out_config);
  EXPECT_TRUE(success);
  int port = out_config.getPort();
  EXPECT_EQ(port, 80);
}

TEST_F(NginxConfigParserTest, largePosPort)
{
  bool success = parser.Parse("./parser_tests/largePosPort_config", &out_config);
  EXPECT_TRUE(success);
  int port = out_config.getPort();
  EXPECT_EQ(port, -1);
}

// when port is simple listen statement
TEST_F(NginxConfigParserTest, validStatementPort)
{
  bool success = parser.Parse("./parser_tests/validStatementPort_config", &out_config);
  EXPECT_TRUE(success);
  int port = out_config.getPort();
  EXPECT_EQ(port, 8080);
}

//when port is nested in block
TEST_F(NginxConfigParserTest, validBlockPort)
{
  bool success = parser.Parse("./parser_tests/validBlockPort_config", &out_config);
  EXPECT_TRUE(success);
  int port = out_config.getPort();
  EXPECT_EQ(port, 8080);
}

TEST_F(NginxConfigParserTest, nonNumericPort1)
{
  bool success = parser.Parse("./parser_tests/nonNumericPort1_config", &out_config);
  EXPECT_TRUE(success);
  int port = out_config.getPort();
  EXPECT_EQ(port, -1);
}

TEST_F(NginxConfigParserTest, nonNumericPort2)
{
  bool success = parser.Parse("./parser_tests/nonNumericPort2_config", &out_config);
  EXPECT_TRUE(success);
  int port = out_config.getPort();
  EXPECT_EQ(port, -1);
}

// test static file mapping
TEST_F(NginxConfigParserTest, mapLocation)
{
  bool success = parser.Parse("./parser_tests/8080_server_config", &out_config);
  EXPECT_TRUE(success);
  out_config.extract_filesystem_map();
  std::map<std::string, std::string> map = out_config.get_filesystem_map();
  std::map<std::string, std::string> expected;
  expected.insert({"/static/help/", "./files/images"});
  expected.insert({"/static/", "./files"});
  expected.insert({"/static2/", "./files/www"});
  EXPECT_EQ(map, expected);
}


// Test ToString for NginxConfig
TEST_F(NginxConfigParserTest, configToString)
{
    bool success = parser.Parse("./parser_tests/string_config", &out_config);
    EXPECT_TRUE(success);
    std::string config_string = out_config.ToString(0);
    std::string expect_string = "listen 80;\nfoo bar;\n";
    EXPECT_STREQ(config_string.c_str(), expect_string.c_str());
}

// ToString with child blocks
TEST_F(NginxConfigParserTest, blockConfigToString)
{
    bool success = parser.Parse("./parser_tests/blockstring_config", &out_config);
    EXPECT_TRUE(success);
    std::string config_string = out_config.ToString(0);
    std::cerr << config_string;
    std::string expect_string = "listen 80;\nfoo bar;\nServer {\n  host 1;\n}\n";
    EXPECT_STREQ(config_string.c_str(), expect_string.c_str());
}
