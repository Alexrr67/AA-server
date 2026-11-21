#pragma once

#include <cstddef>
#include <map>
#include <string>

// Чем закончился разбор запроса.
enum class ParseStatus {
  Ok,    // запрос разобран
  Empty, // пришла пустая строка
  BadRequestLine, // стартовая строка не "МЕТОД ПУТЬ ВЕРСИЯ"
  BadHeader,            // заголовок без двоеточия
  InvalidContentLength, // Content-Length не число
  NeedMoreData,         // запрос пришёл не целиком
};

struct HttpRequest {
  std::string method;                         // GET, POST ...
  std::string path;                           // /health, /items/42
  std::string version;                        // HTTP/1.1
  std::map<std::string, std::string> headers; // ключи в нижнем регистре
  std::string body;                           // тело запроса (JSON)
  bool keep_alive = false; // можно ли использовать соединение дальше

  // Значение заголовка по имени (регистр не важен). Пусто, если нет.
  std::string header(const std::string &name) const;
};

struct ParseResult {
  ParseStatus status = ParseStatus::Empty;
  std::size_t consumed = 0; // сколько байт от начала raw занимает запрос
};

// Разбирает один HTTP-запрос из raw.
ParseResult parse_request(const std::string &raw, HttpRequest &req);

// Текст причины ошибки (для тела ответа 400).
const char *parse_status_message(ParseStatus status);