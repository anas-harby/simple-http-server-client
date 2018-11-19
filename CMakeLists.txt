cmake_minimum_required(VERSION 3.8.2)
project(simple-http-server-client)

set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS -pthread)
set(SERVER_SOURCE_FILES server.cpp server.h working_thread.cpp working_thread.h http/request.cpp http/request.h http/response.cpp http/response.h parser.cpp parser.h filesys.cpp filesys.h)
set(CLIENT_SOURCE_FILES client.cpp client.h client_requests.cpp client_requests.h client_callback.cpp client_callback.h)

find_package(Boost COMPONENTS system filesystem REQUIRED)

if (Boost_FOUND)

    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})

endif ()

add_executable(server ${SERVER_SOURCE_FILES})
add_executable(client ${CLIENT_SOURCE_FILES})
#add_executable(client dummy_client.cpp)

if (Boost_FOUND)

    target_link_libraries(client ${Boost_LIBRARIES})

endif ()
