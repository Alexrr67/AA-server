#include "http/http_request.h"

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

static void expect_status(ParseStatus want, const std::string &raw) {
  HttpRequest req;
  const ParseResult r = parse_request(raw, req);
  if (r.status != want) {
    std::cerr << "FAIL: expected " << parse_status_message(want) << ", got "
              << parse_status_message(r.status) << "\n";
    std::abort();
  }
}

int main() {
  // 1. Пустой запрос - ошибка.
  expect_status(ParseStatus::Empty, "");

  // 2. Запрос без версии - ошибка.
  expect_status(ParseStatus::BadRequestLine, "GET /health\r\n\r\n");

  // 3. Корректный GET /health.
  {
    HttpRequest req;
    const std::string raw = "GET /health HTTP/1.1\r\nHost: localhost\r\n\r\n";
    const ParseResult r = parse_request(raw, req);
    CHECK(r.status == ParseStatus::Ok);
    CHECK(r.consumed == raw.size());
    CHECK(req.method == "GET");
    CHECK(req.path == "/health");
    CHECK(req.version == "HTTP/1.1");
    CHECK(req.header("host") == "localhost");
    CHECK(req.body.empty());
    CHECK(req.keep_alive);
  }

  // 4. POST с Content-Length и телом.
  {
    HttpRequest req;
    const std::string raw =
        "POST /items HTTP/1.1\r\nContent-Length: 5\r\n\r\nhello";
    const ParseResult r = parse_request(raw, req);
    CHECK(r.status == ParseStatus::Ok);
    CHECK(r.consumed == raw.size());
    CHECK(req.method == "POST");
    CHECK(req.body == "hello");
  }

  // 5. Тело ещё не дошло - нужно дочитать.
  expect_status(ParseStatus::NeedMoreData,
                "POST /items HTTP/1.1\r\nContent-Length: 5\r\n\r\nhel");

  // 6. Битый Content-Length.
  expect_status(ParseStatus::InvalidContentLength,
                "POST /items HTTP/1.1\r\nContent-Length: abc\r\n\r\n");

  // 7. Connection: close выключает keep-alive.
  {
    HttpRequest req;
    const ParseResult r =
        parse_request("GET /health HTTP/1.1\r\nConnection: close\r\n\r\n", req);
    CHECK(r.status == ParseStatus::Ok);
    CHECK(!req.keep_alive);
  }

  // 8. Два запроса подряд: consumed показывает, где кончается первый.
  {
    HttpRequest req;
    const std::string first = "GET /health HTTP/1.1\r\n\r\n";
    const ParseResult r = parse_request(first + first, req);
    CHECK(r.status == ParseStatus::Ok);
    CHECK(r.consumed == first.size());
  }

  std::cout << "all parser tests passed\n";
  return 0;
}