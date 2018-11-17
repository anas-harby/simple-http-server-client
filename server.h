#ifndef SIMPLE_HTTP_SERVER_CLIENT_SERVER_H
#define SIMPLE_HTTP_SERVER_CLIENT_SERVER_H

#include <thread>
#include <map>
#include <mutex>
#include "working_thread.h"

class server {

public:
    const int MAX_CONNECTIONS = 100;

    server(int port_number);
    void start();

private:
    int port_number, socket_fd;
    struct sockaddr_in address;
    std::map<std::thread::id, working_thread *> working_threads;
    std::mutex threads_mtx;

    void init();
    void dispatch(int connection_socket);
    void handle_request(int connection_socket);
    void cleanup_working_threads();
};


#endif //SIMPLE_HTTP_SERVER_CLIENT_SERVER_H
