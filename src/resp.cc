#include "charconv"

#include "resp.h"
#include "overloaded.h"


// parse *2\r\n$4\r\nECHO\r\n$5\r\nhello\r\n
std::optional<ParsedCommand> parse_command(std::string_view buffer) {
  // guards against injection attack to exploit memory by reserving
  // absurd amount 

  if (buffer.empty() || buffer.front() != '*') return std::nullopt;

  std::vector<std::string> elements;
  
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

  elements.reserve(n);

  pos = line_end + 2; // skip \r\n
                      //
  for (int i = 0; i < n; i++) {
    if (pos >= buffer.size() || buffer[pos] != '$') return std::nullopt;

    pos++; // skip $

    line_end = buffer.find("\r\n", pos);
    if (line_end == std::string::npos) return std::nullopt;

    
    size_t el_len = 0;
    auto [ptr, ec] = std::from_chars(buffer.data() + pos, buffer.data() + line_end, el_len);
    if (ec != std::errc{}) return std::nullopt;

    if (el_len > kMaxBulkLen) return std::nullopt;

    pos = line_end + 2;

    if (pos + el_len + 2 > buffer.size() || buffer.substr(pos + el_len, 2) != "\r\n") return std::nullopt;

    std::string element;
    element.reserve(el_len);
    element.append(buffer.substr(pos, el_len));
    elements.emplace_back(std::move(element));

    pos += el_len + 2;

  }

  ParsedCommand result = {};
  result.name = std::move(elements.front());
  result.args.assign(std::make_move_iterator(elements.begin() + 1),
                      std::make_move_iterator(elements.end()));
  
  return std::optional(result);
}

std::string serialize(const RespValue& v) {
    return std::visit(overloaded{
        [](const SimpleString& s) { return "+" + s.value + "\r\n"; },
        [](const BulkString& b)   { return "$" + std::to_string(b.value.size()) + "\r\n" + b.value + "\r\n"; },
        [](const RespError& e)    { return "-ERR " + e.message + "\r\n"; },
        [](const Nil&)            { return std::string("$-1\r\n"); },
    }, v);
}
