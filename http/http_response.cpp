#include "http_response.h"

#include <string>

const char *status_text(int status) {
  switch (status) {
  case 200:
    return "OK";
  case 201:
    return "Created";
  case 204:
    return "No Content";
  case 400:
    return "Bad Request";
  case 404:
    return "Not Found";
  case 405:
    return "Method Not Allowed";
  case 409:
    return "Conflict";
  case 500:
    return "Internal Server Error";
  default:
    return "Unknown";
  }
}

std::string serialize_response(const HttpResponse &resp) {
  std::string out;
  out += "HTTP/1.1 ";
  out += std::to_string(resp.status);
  out += " ";
  out += status_text(resp.status);
  out += "\r\n";
  out += "Content-Type: " + resp.content_type + "\r\n";
  out += "Content-Length: " + std::to_string(resp.body.size()) + "\r\n";
  out += "\r\n";
  out += resp.body;
  return out;
}