cmake_minimum_required(VERSION 3.12)
project(simple-http-server-client)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost COMPONENTS system filesystem REQUIRED)

if (Boost_FOUND)

    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_VERSION: ${Boost_VERSION}")

    include_directories(${Boost_INCLUDE_DIRS})

endif ()

add_executable(client client.cpp client.h client_requests.cpp client_requests.h client_callback.cpp client_callback.h)

if (Boost_FOUND)

    target_link_libraries(client ${Boost_LIBRARIES})

endif ()