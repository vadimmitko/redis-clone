#include "resp.h"
#include <gtest/gtest.h>

class RespTest : public testing::Test {}; 

TEST_F(RespTest, ParserSuccessCorrectFormat) {
  const char* testString = "*2\r\n$4\r\nECHO\r\n$5\r\nhello\r\n";

  std::optional<ParsedCommand> parsed_o = parse_command(testString);

  EXPECT_TRUE(parsed_o.has_value());
  EXPECT_EQ(parsed_o->name, "ECHO");
  EXPECT_EQ(parsed_o->args.size(), 1);
  EXPECT_EQ(parsed_o->args.front(), "hello");
  
}

TEST_F(RespTest, ParserNulloptIncorrectArgsNumber) {
  const char* testString = "*10\r\n$4\r\nECHO\r\n$5\r\nhello\r\n";

  std::optional<ParsedCommand> parsed_o = parse_command(testString);

  EXPECT_FALSE(parsed_o.has_value());
}

TEST_F(RespTest, ParserNulloptIncorrectBytesNumber) {
  const char* testString = "*1\r\n$6\r\nPING";

  std::optional<ParsedCommand> parsed_o = parse_command(testString);

  EXPECT_FALSE(parsed_o.has_value());
}


TEST_F(RespTest, ParserNulloptTooManyArgs) {
  const char* testString = "*9999999999999999\r\n$6\r\nECHO\r\n$5\r\nhello\r\n";

  std::optional<ParsedCommand> parsed_o = parse_command(testString);

  EXPECT_FALSE(parsed_o.has_value());
}

TEST_F(RespTest, ParserNulloptTooManyBytes) {
  const char* testString = "*2\r\n$9999999999999999\r\nECHO\r\n$5\r\nhello\r\n";

  std::optional<ParsedCommand> parsed_o = parse_command(testString);

  EXPECT_FALSE(parsed_o.has_value());
}
