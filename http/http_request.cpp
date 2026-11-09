#include "http_request.h"
#include <sstream>

bool parse_request(const std::string &raw, HttpRequest &req) {
  std::istringstream in(raw);
  std::string line;
  if (!std::getline(in, line))
    return false;
  std::istringstream ls(line);
  ls >> req.method >> req.path >> req.version;
  return !req.method.empty() && !req.path.empty();
}