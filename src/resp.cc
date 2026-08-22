#include <charconv>
#include <optional>
#include <string>

#include "resp.h"
#include "overloaded.h"


// parse *2\r\n$4\r\nECHO\r\n$5\r\nhello\r\n
std::optional<std::vector<std::string>> parse_tokens(std::string_view buffer) {
  // guards against injection attack to exploit memory by reserving
  // absurd amount 

  if (buffer.empty() || buffer.front() != '*') return std::nullopt;

  std::vector<std::string> tokens;
  
  size_t pos = 1; // skip *
  size_t line_end = buffer.find("\r\n", pos);
  if (line_end == std::string::npos) {
    return std::nullopt;
  }

  size_t n = 0;
  auto [ptr, ec] = std::from_chars(buffer.data() + pos, buffer.data() + line_end, n);
  if (ec != std::errc{})
      return std::nullopt;

  if (n > kMaxMultibulkLen) return std::nullopt;

  tokens.reserve(n);

  pos = line_end + 2; // skip \r\n
                      //
  for (size_t i = 0; i < n; i++) {
    if (pos >= buffer.size() || buffer[pos] != '$') return std::nullopt;

    pos++; // skip $

    line_end = buffer.find("\r\n", pos);
    if (line_end == std::string::npos) return std::nullopt;

    
    size_t token_len = 0;
    auto [ptr, ec] = std::from_chars(buffer.data() + pos, buffer.data() + line_end, token_len);
    if (ec != std::errc{}) return std::nullopt;

    if (token_len > kMaxBulkLen) return std::nullopt;

    pos = line_end + 2;

    if (pos + token_len + 2 > buffer.size() || buffer.substr(pos + token_len, 2) != "\r\n") return std::nullopt;

    std::string token;
    token.reserve(token_len);
    token.append(buffer.substr(pos, token_len));
    tokens.emplace_back(std::move(token));

    pos += token_len + 2;

  }

  return std::move(tokens);
}

std::optional<ParsedCommand> parse_command(std::string_view buffer) {
  std::optional<std::vector<std::string>> tokens_o = parse_tokens(buffer);

  if (!tokens_o.has_value()) return std::nullopt;

  ParsedCommand result = {};
  result.name = std::move(tokens_o.value().front());
  result.args.assign(std::make_move_iterator(tokens_o.value().begin() + 1),
                      std::make_move_iterator(tokens_o.value().end()));
  
  return result;
}

std::string serialize(const RespValue& v) {
    return std::visit(overloaded{
        [](const Integer& i) { return ':' + std::to_string(i.n) + "\r\n"; },
        [](const Array& a) { 
          std::string arr;
          for (const std::string& s : a.v) {
            arr += "$" + std::to_string(s.size()) + "\r\n" + s + "\r\n";
          }
          return "*" + std::to_string(a.v.size()) + "\r\n" + arr;
        },
        [](const SimpleString& s) { return "+" + s.value + "\r\n"; },
        [](const BulkString& b)   { return "$" + std::to_string(b.value.size()) + "\r\n" + b.value + "\r\n"; },
        [](const RespError& e)    { return "-ERR " + e.message + "\r\n"; },
        [](const Nil&)            { return std::string("$-1\r\n"); },
    }, v);
}
