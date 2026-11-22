#include "http_request.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace {

std::string to_lower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
    return static_cast<char>(std::tolower(c));
  });
  return s;
}

std::string trim(const std::string &s) {
  const auto begin = s.find_first_not_of(" \t\r\n");
  if (begin == std::string::npos) {
    return "";
  }
  const auto end = s.find_last_not_of(" \t\r\n");
  return s.substr(begin, end - begin + 1);
}

bool is_number(const std::string &s) {
  return !s.empty() && std::all_of(s.begin(), s.end(), [](unsigned char c) {
    return std::isdigit(c) != 0;
  });
}

} // namespace

std::string HttpRequest::header(const std::string &name) const {
  const auto it = headers.find(to_lower(name));
  return it == headers.end() ? std::string() : it->second;
}

ParseResult parse_request(const std::string &raw, HttpRequest &req) {
  req = HttpRequest{};
  ParseResult result;

  if (raw.empty()) {
    result.status = ParseStatus::Empty;
    return result;
  }

  // 1. Ищем конец блока заголовков: пустую строку.
  std::size_t header_end = raw.find("\r\n\r\n");
  std::size_t separator_len = 4;
  if (header_end == std::string::npos) {
    header_end = raw.find("\n\n");
    separator_len = 2;
  }
  if (header_end == std::string::npos) {
    result.status = ParseStatus::NeedMoreData;
    return result;
  }

  // 2. Стартовая строка и заголовки.
  std::istringstream in(raw.substr(0, header_end));
  std::string line;

  if (!std::getline(in, line)) {
    result.status = ParseStatus::Empty;
    return result;
  }
  if (!line.empty() && line.back() == '\r') {
    line.pop_back();
  }

  {
    std::istringstream ls(line);
    std::string extra;
    if (!(ls >> req.method >> req.path >> req.version) || (ls >> extra)) {
      result.status = ParseStatus::BadRequestLine;
      return result;
    }
  }

  if (req.version.rfind("HTTP/1.", 0) != 0) {
    result.status = ParseStatus::BadRequestLine;
    return result;
  }

  while (std::getline(in, line)) {
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }
    if (line.empty()) {
      break;
    }

    const auto colon = line.find(':');
    if (colon == std::string::npos) {
      result.status = ParseStatus::BadHeader;
      return result;
    }
    const std::string name = to_lower(trim(line.substr(0, colon)));
    if (name.empty()) {
      result.status = ParseStatus::BadHeader;
      return result;
    }
    req.headers[name] = trim(line.substr(colon + 1));
  }

  // 3. Content-Length и тело.
  std::size_t content_length = 0;
  const auto cl = req.headers.find("content-length");
  if (cl != req.headers.end()) {
    if (!is_number(cl->second)) {
      result.status = ParseStatus::InvalidContentLength;
      return result;
    }
    try {
      content_length = static_cast<std::size_t>(std::stoul(cl->second));
    } catch (...) {
      result.status = ParseStatus::InvalidContentLength;
      return result;
    }
  }

  const std::size_t body_start = header_end + separator_len;
  if (raw.size() < body_start + content_length) {
    result.status = ParseStatus::NeedMoreData;
    return result;
  }
  req.body = raw.substr(body_start, content_length);

  // 4. keep-alive: HTTP/1.1 по умолчанию да, HTTP/1.0 - нет.
  const std::string connection = to_lower(req.header("connection"));
  if (!connection.empty()) {
    req.keep_alive = (connection != "close");
  } else {
    req.keep_alive = (req.version == "HTTP/1.1");
  }

  result.status = ParseStatus::Ok;
  result.consumed = body_start + content_length;
  return result;
}

const char *parse_status_message(ParseStatus status) {
  switch (status) {
  case ParseStatus::Ok:
    return "ok";
  case ParseStatus::Empty:
    return "empty request";
  case ParseStatus::BadRequestLine:
    return "bad request line";
  case ParseStatus::BadHeader:
    return "bad header";
  case ParseStatus::InvalidContentLength:
    return "invalid Content-Length";
  case ParseStatus::NeedMoreData:
    return "incomplete request";
  }
  return "unknown error";
}