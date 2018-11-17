#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.h"
#include <cstring>

int read_port_number(int argc, char **args) {
    if (argc != 2) {
        std::cerr << "Invalid number of arguments!";
        exit(EXIT_FAILURE);
    }
    return std::stoi(args[argc - 1]);
}

server::server(int port_number) {
    server::port_number = port_number;
    server::address = {};
    server::init();
}

void server::init() {
    // Creating socket file descriptor
    if ((server::socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port
    int opt = 1;
    if (setsockopt(server::socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Set address fields
    server::address.sin_family = AF_INET;
    server::address.sin_addr.s_addr = INADDR_ANY;
    server::address.sin_port = htons(static_cast<uint16_t>(server::port_number));

    // Assigns the local internet address and port for a socket
    if (bind(server::socket_fd, (struct sockaddr *) &(server::address), sizeof(server::address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Initiate working thread cleanup service
    std::thread *th = new std::thread(&server::cleanup_working_threads, this);
    working_thread *th_cleanup = new working_thread(th);
    th_cleanup->detach();
}

void server::start() {
    if (listen(server::socket_fd, server::MAX_CONNECTIONS) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (true) {
        int connection_socket;
        int address_len = sizeof(server::address);
        if ((connection_socket = accept(server::socket_fd,
                                        (struct sockaddr *)&(server::address),(socklen_t*)&address_len)) < 0)
            continue;

        server::dispatch(connection_socket);
    }
}

void server::handle_request(int connection_socket) {
    std::cout << std::this_thread::get_id() << " TEST\n";
    std::cout << connection_socket << " Conn\n";

    server::threads_mtx.lock();
    server::working_threads[std::this_thread::get_id()]->mark_done();
    server::threads_mtx.unlock();
}

void server::dispatch(int connection_socket) {
    std::thread *th = new std::thread(&server::handle_request, this, connection_socket);
    working_thread *wrk_th = new working_thread(th);

    server::threads_mtx.lock();
    server::working_threads[wrk_th->get_thread_id()] = wrk_th;
    server::threads_mtx.unlock();

    wrk_th->detach();
}

void server::cleanup_working_threads() {
    while (true) {
        server::threads_mtx.lock();
        // cleanup resources.
        for (auto it = server::working_threads.cbegin(); it != server::working_threads.cend();) {
            if (it->second->is_done()) {
                delete it->second;
                server::working_threads.erase(it++);
            } else
                it++;
        }

        server::threads_mtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main(int argc, char **args) {
    int port_number = read_port_number(argc, args);
    std::cout << port_number << std::endl;
    server serv(port_number);
    serv.start();
    return 0;
}
