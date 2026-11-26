#include "handlers/handler.h"

HttpResponse handle_request(const HttpRequest &req) {
  if (req.path == "/health") {
    if (req.method == "GET") {
      return HttpResponse{200, "application/json", R"({"status":"ok"})"};
    }
    return HttpResponse{405, "application/json",
                        R"({"error":"method not allowed"})"};
  }
  return HttpResponse{404, "application/json", R"({"error":"not found"})"};
}