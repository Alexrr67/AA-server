#pragma once

#include "http/http_request.h"
#include "http/http_response.h"

HttpResponse handle_request(const HttpRequest &req);