#include "handlers/handler.h"
#include "http/http_request.h"
#include "http/http_response.h"

#include <cstdlib>
#include <iostream>
#include <string>

#define CHECK(cond)                                                            \
  do {                                                                         \
    if (!(cond)) {                                                             \
      std::cerr << "FAIL: " #cond " (line " << __LINE__ << ")\n";              \
      std::abort();                                                            \
    }                                                                          \
  } while (0)

static HttpRequest make(const std::string &method, const std::string &path) {
  HttpRequest req;
  req.method = method;
  req.path = path;
  req.version = "HTTP/1.1";
  return req;
}

int main() {
  {
    const HttpResponse r = handle_request(make("GET", "/health"));
    CHECK(r.status == 200);
    CHECK(r.content_type == "application/json");
    CHECK(r.body == R"({"status":"ok"})");
  }
  CHECK(handle_request(make("POST", "/health")).status == 405);
  CHECK(handle_request(make("GET", "/unknown")).status == 404);

  {
    const HttpResponse r{200, "application/json", R"({"status":"ok"})"};
    const std::string text = serialize_response(r);
    CHECK(text.rfind("HTTP/1.1 200 OK\r\n", 0) == 0);
    CHECK(text.find("Content-Type: application/json\r\n") != std::string::npos);
    CHECK(text.find("Content-Length: 15\r\n") != std::string::npos);
    CHECK(text.find("\r\n\r\n") != std::string::npos);
    CHECK(text.substr(text.size() - 15) == R"({"status":"ok"})");
  }

  std::cout << "all handler tests passed\n";
  return 0;
}