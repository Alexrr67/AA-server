#pragma once
#include <string>

struct HttpRequest {
  std::string method;
  std::string path;
  std::string version;
};

bool parse_request(const std::string &raw, HttpRequest &req);