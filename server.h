#ifndef SIMPLE_HTTP_SERVER_CLIENT_SERVER_H
#define SIMPLE_HTTP_SERVER_CLIENT_SERVER_H

#include <thread>
#include <map>
#include <mutex>
#include "working_thread.h"

class server {

public:
    const int MAX_CONNECTIONS = 100;
    const float ALPHA_TIMEOUT_MOVING_AVERAGE = 0.25;
    const unsigned long INITIAL_TIMEOUT = 5000;

    server(int port_number);
    void start();

private:
    unsigned int port_number, socket_fd;
    struct sockaddr_in address;

    unsigned long cur_timeout;
    std::mutex timeout_mtx;

    std::map<std::thread::id, working_thread *> working_threads;
    std::mutex threads_mtx;

    void init();
    void dispatch(int connection_socket);
    void handle_request(int connection_socket);
    void cleanup_working_threads();
    void finalize_connection(int connection_socket);
    struct timeval get_tv_from_timeout();
};


#endif //SIMPLE_HTTP_SERVER_CLIENT_SERVER_H
