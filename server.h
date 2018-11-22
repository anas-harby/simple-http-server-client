#ifndef SIMPLE_HTTP_SERVER_CLIENT_SERVER_H
#define SIMPLE_HTTP_SERVER_CLIENT_SERVER_H

#include <thread>
#include <map>
#include <mutex>
#include "working_thread.h"

class server {

public:
    const int MAX_CONNECTIONS = 1000;
    const unsigned long CLEANUP_RATE = 500;
    const unsigned long INITIAL_TIMEOUT = 20000;
    const float TIMEOUT_DECAY_RATE = 0.01f;

    server(int port_number);
    void start();

private:
    unsigned int port_number, socket_fd;
    struct sockaddr_in address;

    unsigned long cur_timeout;

    std::map<std::thread::id, working_thread *> working_threads;
    std::mutex threads_mtx;

    void init();
    void dispatch(int connection_socket);
    void handle_request(int connection_socket);
    void cleanup_working_threads();
    void finalize_connection(int connection_socket);
    int get_number_of_connections();
    void update_timeout();
    struct timeval get_tv_from_timeout();
};


#endif //SIMPLE_HTTP_SERVER_CLIENT_SERVER_H
