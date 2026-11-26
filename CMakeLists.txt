cmake_minimum_required(VERSION 3.20)
project(http_server)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- сервер ---
add_executable(server
  main.cpp
  http/http_request.cpp
  http/http_response.cpp
  handlers/handler.cpp
)
target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# --- тесты ---
enable_testing()
add_executable(test_parser tests/test_parser.cpp http/http_request.cpp)
target_include_directories(test_parser PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME parser COMMAND test_parser)

add_executable(test_handler
  tests/test_handler.cpp
  handlers/handler.cpp
  http/http_response.cpp
)
target_include_directories(test_handler PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME handler COMMAND test_handler)