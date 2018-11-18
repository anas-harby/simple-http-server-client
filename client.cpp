//
// Created by heshamovic on 11/16/18.
//

#include "client.h"
#include "client_requests.h"
#include "client_callback.h"
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

template<typename Char>
client::net::basic_socketbuf<Char>::basic_socketbuf() : sock(0) {
    buf_type::setp(obuf, obuf + (SIZE - 1));
    buf_type::setg(ibuf, ibuf, ibuf);
}

template<typename Char>
client::net::basic_socketbuf<Char>::~basic_socketbuf() { sync(); }

template<typename Char>
void client::net::basic_socketbuf<Char>::set_socket(int sock) { this->sock = sock; }

template<typename Char>
int client::net::basic_socketbuf<Char>::get_socket() { return this->sock; }

template<typename Char>
int client::net::basic_socketbuf<Char>::output_buffer() {
    int num = buf_type::pptr() - buf_type::pbase();
    if (send(sock, reinterpret_cast<char *>(obuf), num * char_size, 0) != num)
        return traits_type::eof();
    buf_type::pbump(-num);
    return num;
}


template<typename Char>
int client::net::basic_socketbuf<Char>::sync() {
    if (output_buffer() == traits_type::eof())
        return traits_type::eof();
    return 0;
}

template<typename Char>
client::net::basic_socketstream<Char>::basic_socketstream()  : stream_type(&buf) {}

template<typename Char>
client::net::basic_socketstream<Char>::basic_socketstream(int s) : stream_type(&buf) { buf.set_socket(s); }

template<typename Char>
void client::net::basic_socketstream<Char>::close_socket() {
    if (buf.get_socket() != 0)
        close(buf.get_socket());
    stream_type::clear();
}

template<typename Char>
bool client::net::basic_socketstream<Char>::open(const std::string &host, uint16_t port) {
    int status;
    close_socket();
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in sin;
    hostent *he = gethostbyname(host.c_str());

    std::copy(reinterpret_cast<char *>(he->h_addr), reinterpret_cast<char *>(he->h_addr) + he->h_length,
              reinterpret_cast<char *>(&sin.sin_addr.s_addr));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);

    if (status = connect(sd, reinterpret_cast<sockaddr *>(&sin), sizeof(sin)) < 0)
        stream_type::setstate(std::ios::failbit);
    else
        buf.set_socket(sd);
    return !status;
}

void validate_arguments(int argc, const char **argv) {
    if (argc != 4) {
        std::cerr << "Invalid number of arguments!\n";
        exit(EXIT_FAILURE);
    }
}

int main(int argc, const char *argv[]) {

    int port;
    validate_arguments(argc, argv);
    std::string server_ip = argv[argc - 3];
    client::net::socketstream ss;
    std::ifstream inFile;
    std::string s;


    inFile.open(argv[argc - 1]);
    if (!inFile) {
        std::cerr << "Unable to open file";
        exit(1);
    }

    request::init();
    callback::init();

    while (std::getline(inFile, s)) {
        std::vector<std::string> results;
        boost::split(results, s, [](char c) { return c == ' '; });

        if (results.size() == 4) {
            port = std::stoi(results[3]);
            results.reserve(3);
        } else
            port = client::default_port;
        if (ss.open(server_ip, port)) {

            (*requestMap[results[0]])(results, ss);

            (*callbackMap[results[0]])(results, ss);
        }
    }


    return 0;
}

