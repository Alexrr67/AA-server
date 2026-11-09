#include "http/http_request.h"
#include <cassert>
#include <iostream>

int main() {
  HttpRequest r;
  assert(parse_request("GET /health HTTP/1.1\r\n\r\n", r));
  assert(r.method == "GET");
  assert(r.path == "/health");
  std::cout << "parser OK\n";
  return 0;
}