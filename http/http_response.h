#pragma once

#include <string>

struct HttpResponse {
  int status = 200;
  std::string content_type = "application/json";
  std::string body;
};

const char *status_text(int status);
std::string serialize_response(const HttpResponse &resp);