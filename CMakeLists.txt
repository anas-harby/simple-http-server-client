cmake_minimum_required(VERSION 3.8)
project(simple_http_server_client)

set(CMAKE_CXX_STANDARD 14)

set(SERVER_SOURCE_FILES server.cpp server.h working_thread.cpp working_thread.h)
set(CLIENT_SOURCE_FILES dummy_client.cpp )
add_executable(server ${SERVER_SOURCE_FILES})
add_executable(client ${CLIENT_SOURCE_FILES})