#include "gtest/gtest.h"
#include "config_parser.h"
#include <string>

class NginxConfigParserTest : public ::testing::Test {
  protected:
    NginxConfigParser parser;
    NginxConfig out_config;	
};


// SimpleConfig testcase
TEST_F(NginxConfigParserTest, SimpleConfig) {
  bool success = parser.Parse("example_config", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, TokensConfig) {
  bool success = parser.Parse("tokens_config", &out_config);
  EXPECT_TRUE(success);
}

// Syntax checks
TEST_F(NginxConfigParserTest, BadSyntax1) {
  bool success = parser.Parse("syntax1_config", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, BadSyntax2) {
  bool success = parser.Parse("syntax2_config", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, BadSyntax3) {
  bool success = parser.Parse("syntax3_config", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, BadSyntax4) {
  bool success = parser.Parse("syntax4_config", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, BadSyntax5) {
  bool success = parser.Parse("syntax5_config", &out_config);
  EXPECT_FALSE(success);
}

// Empty config file should return true
TEST_F(NginxConfigParserTest, EmptyConfig) {
  bool success = parser.Parse("empty_config", &out_config);
  EXPECT_TRUE(success);
}

// Empty block should work with grammar
TEST_F(NginxConfigParserTest, EmptyBlock) {
  bool success = parser.Parse("emptyblock_config", &out_config);
  EXPECT_TRUE(success);
}


TEST_F(NginxConfigParserTest, Whitespace) {
  bool success = parser.Parse("whitespace_config", &out_config);
  EXPECT_FALSE(success);
}

TEST_F(NginxConfigParserTest, singleEscape) {
  bool success = parser.Parse("singleEscape_config", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, doubleEscape) {
  bool success = parser.Parse("doubleEscape_config", &out_config);
  EXPECT_TRUE(success);
}

TEST_F(NginxConfigParserTest, ConfigWithComment) {
    bool success = parser.Parse("comment_config", &out_config);
    EXPECT_TRUE(success);
}

// Testing port number test cases
TEST_F(NginxConfigParserTest, negativePort) {
  bool success = parser.Parse("negativePort_config", &out_config);
  EXPECT_TRUE(success);
  int port = out_config.getPort();
  EXPECT_EQ(port, -1);
}

TEST_F(NginxConfigParserTest, reservedPort) {
  bool success = parser.Parse("reservedPort_config", &out_config);
  EXPECT_TRUE(success);
  int port = out_config.getPort();
  EXPECT_EQ(port, 80);
}

TEST_F(NginxConfigParserTest, largePosPort) {
  bool success = parser.Parse("largePosPort_config", &out_config);
  EXPECT_TRUE(success);
  int port = out_config.getPort();
  EXPECT_EQ(port, -1);
}

// when port is simple listen statement
TEST_F(NginxConfigParserTest, validStatementPort) {
  bool success = parser.Parse("validStatementPort_config", &out_config);
  EXPECT_TRUE(success);
  int port = out_config.getPort();
  EXPECT_EQ(port, 8080);
}

//when port is nested in block
TEST_F(NginxConfigParserTest, validBlockPort) {
  bool success = parser.Parse("validBlockPort_config", &out_config);
  EXPECT_TRUE(success);
  int port = out_config.getPort();
  EXPECT_EQ(port, 8080);
}

TEST_F(NginxConfigParserTest, nonNumericPort1) {
  bool success = parser.Parse("nonNumericPort1_config", &out_config);
  EXPECT_TRUE(success);
  int port = out_config.getPort();
  EXPECT_EQ(port, -1);
}

TEST_F(NginxConfigParserTest, nonNumericPort2) {
  bool success = parser.Parse("nonNumericPort2_config", &out_config);
  EXPECT_TRUE(success);
  int port = out_config.getPort();
  EXPECT_EQ(port, -1);
}


// Test ToString for NginxConfig
TEST_F(NginxConfigParserTest, configToString) {
    bool success = parser.Parse("string_config", &out_config);
    EXPECT_TRUE(success);
    std::string config_string = out_config.ToString(0);
    std::string expect_string = "listen 80;\nfoo bar;\n";
    EXPECT_STREQ(config_string.c_str(), expect_string.c_str());
}

// ToString with child blocks
TEST_F(NginxConfigParserTest, blockConfigToString) {
    bool success = parser.Parse("blockstring_config", &out_config);
    EXPECT_TRUE(success);
    std::string config_string = out_config.ToString(0);
    std::cerr << config_string;
    std::string expect_string = "listen 80;\nfoo bar;\nServer {\n  host 1;\n}\n";
    EXPECT_STREQ(config_string.c_str(), expect_string.c_str());
}
