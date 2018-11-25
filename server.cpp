#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.h"
#include "parser.h"
#include <cstring>
#include <sstream>
#include <math.h>
#include <unistd.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>

fd_set build_new_fd_set(int connection_socket);

server::server(int port_number) {
    server::port_number = port_number;
    server::cur_timeout = INITIAL_TIMEOUT;
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
    // select(): tries to find if there is a data that can be received
    // within the timeout span currently set.
    fd_set rfds = build_new_fd_set(connection_socket);
    while (true) {
        struct timeval tv = server::get_tv_from_timeout();
        int retval = select(connection_socket + 1, &rfds, NULL, NULL, &tv);

        if (!retval) {
            perror ((std::string("Timeout reached for connection: ")
                     + std::to_string(connection_socket)).c_str());

            http_response res = parser::get_timeout_response();
            std::cout << res << std::endl;

            std::stringstream res_ss; res_ss << res;
            send(connection_socket, res_ss.str().c_str(), res_ss.str().size(), 0);
            server::finalize_connection(connection_socket);
            return;
        }

        // Parse request and print it to console
        bool keep_alive = true;
        std::vector<http_request> requests = parser::parse(connection_socket);
        for (auto req : requests) {
            std::cout << req << std::endl;

            // Formulate response and print it to console
            http_response res = parser::get_response(req);
            std::cout << res << std::endl;

            // Send response back to client through connection socket
            std::stringstream res_ss;
            res_ss << res;
            send(connection_socket, res_ss.str().c_str(), res_ss.str().length(), 0);

            keep_alive &= req.header_exists("Connection") && req.get_header_value("Connection") == "keep-alive";
            if (!keep_alive)
                break;
        }
        if (!keep_alive)
            break;
    }
    server::finalize_connection(connection_socket);
}

void server::dispatch(int connection_socket) {
    std::thread *th = new std::thread(&server::handle_request, this, connection_socket);
    working_thread *wrk_th = new working_thread(th);

    server::threads_mtx.lock();
    server::working_threads[wrk_th->get_thread_id()] = wrk_th;
    server::update_timeout();
    server::threads_mtx.unlock();

    wrk_th->detach();
}

void server::cleanup_working_threads() {
    while (true) {
        server::threads_mtx.lock();
        for (auto it = server::working_threads.cbegin();
                it != server::working_threads.cend();) {
            if (it->second->is_done()) {
                delete it->second;
                server::working_threads.erase(it++);
            } else
                it++;
        }
        server::update_timeout();
        server::threads_mtx.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int server::get_number_of_connections() {
    return server::working_threads.size();
}

void server::finalize_connection(int connection_socket) {
    close(connection_socket);
    
    server::threads_mtx.lock();
    server::working_threads[std::this_thread::get_id()]->mark_done();
    server::threads_mtx.unlock();
}

struct timeval server::get_tv_from_timeout() {
    struct timeval tv;

    server::threads_mtx.lock();
    tv.tv_sec = server::cur_timeout / 1000;
    tv.tv_usec = (server::cur_timeout % 1000) * 1000;
    server::threads_mtx.unlock();

    return tv;
}

void server::update_timeout() {
    server::cur_timeout = server::INITIAL_TIMEOUT *
        pow((1 - server::TIMEOUT_DECAY_RATE), server::get_number_of_connections());
}

fd_set build_new_fd_set(int connection_socket) {
    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(connection_socket, &rfds);
    return rfds;
}


int read_port_number(int argc, char **args) {
    if (argc != 2) {
        std::cerr << "Invalid number of arguments!\n";
        exit(EXIT_FAILURE);
    }
    return std::stoi(args[argc - 1]);
}

int main(int argc, char **args) {
    int port_number = read_port_number(argc, args);
    std::cout << "Server listening on port: " << port_number <<  std::endl;
    server _server(port_number);
    _server.start();
    return 0;
}
