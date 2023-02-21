#include <iostream>
#include <regex>
#include <string>
namespace helper {
auto SplitIPv4(std::string text) -> std::vector<std::string> {
  std::regex ws_re("(\\d+.){3}\\d+");
  std::vector<std::string> vector(std::sregex_token_iterator(text.begin(), text.end(), ws_re, -1),
                                  std::sregex_token_iterator());
  return vector;
}
auto Split(std::string text) -> std::vector<std::string> {
  std::regex ws_re("\\s+");
  std::vector<std::string> vector(std::sregex_token_iterator(text.begin(), text.end(), ws_re, -1),
                                  std::sregex_token_iterator());
  return vector;
}
}  // namespace helper
