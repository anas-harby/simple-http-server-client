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
    working_thread *th_cleanup = new working_thread(&server::cleanup_working_threads);
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
        break;
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
    working_thread *th = new working_thread(&server::handle_request, connection_socket);

    server::threads_mtx.lock();
    server::working_threads[th->get_thread_id()] = th;
    server::threads_mtx.unlock();

    th->detach();
}

void server::cleanup_working_threads() {
    server::threads_mtx.lock();
    // cleanup resources.
    for (auto it = server::working_threads.cbegin(); it != server::working_threads.cend();) {
        if (it->second->is_done()) {
            delete it->second;
            server::working_threads.erase(it++);
        }
        else
            it++;
    }

    server::threads_mtx.unlock();
    // sleep for 500ms
}

//int Server::get_port_number() const {
//    return Server::port_number;
//}
//
//void Server::set_port_number(int port_number) {
//    Server::port_number = port_number;
//}
//

int main(int argc, char **args) {
    int port_number = read_port_number(argc, args);
    std::cout << port_number << std::endl;
    server serv(port_number);
    serv.start();
    return 0;
}

//int main(int argc, char **args) {
//    int port_number = read_port_number(argc, args);
//    std::cout << port_number << std::endl;
//    auto *server = new Server(port_number);
//
//    // Variables Definitions
//    int server_fd, connection_socket, value_read;
//    struct sockaddr_in address{};
//    int opt = 1;
//    int address_len = sizeof(address);
//    char buffer[1024] = {0};
//    auto *hello = const_cast<char *>("Hello from server");
//
//    // Creating socket file descriptor
//    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
//    {
//        perror("socket failed");
//        exit(EXIT_FAILURE);
//    }
//
//    // Forcefully attaching socket to the port
//    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
//                   &opt, sizeof(opt)))
//    {
//        perror("setsockopt");
//        exit(EXIT_FAILURE);
//    }
//
//    // Set address fields
//    address.sin_family = AF_INET;
//    address.sin_addr.s_addr = INADDR_ANY;
//    address.sin_port = htons(static_cast<uint16_t>(server->get_port_number()));
//
//    // Assigns the local internet address and port for a socket
//    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
//    {
//        perror("bind failed");
//        exit(EXIT_FAILURE);
//    }
//
//    if (listen(server_fd, 3) < 0) // Max 3 connections
//    {
//        perror("listen");
//        exit(EXIT_FAILURE);
//    }
//
//    if ((connection_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&address_len)) < 0)
//    {
//        perror("accept");
//        exit(EXIT_FAILURE);
//    }
//
//    value_read = static_cast<int>(read(connection_socket , buffer, 1024));
//    std::cout << buffer << std::endl;
//    send(connection_socket , hello, strlen(hello) , 0);
//    std::cout << "Hello message sent"  << std::endl;
//    return 0;
//}