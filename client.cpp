#include "client.h"
#include "client_requests.h"
#include "client_callback.h"
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include "parser.h"
#include "filesys.h"

Semaphore sem;


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
    if (send(sock, reinterpret_cast<char *>(obuf), num * char_size, 0) != num) {
        return traits_type::eof();
    }
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
    if (argc != 4 && argc != 5) {
        std::cerr << "Invalid number of arguments!\n";
        exit(EXIT_FAILURE);
    }
}

int main(int argc, const char *argv[]) {

    validate_arguments(argc, argv);
    bool pipeline = (argc == 5 && !std::string(argv[4]).compare("1.1"));

    callback::init();

    if (pipeline)
        client::http1_1(argv);
    else
        client::http1_0(argv);

    return 0;
}


void client::http1_0(const char *argv[]) {

    int port;
    std::string server_ip = argv[1];
    client::net::socketstream ss;
    std::ifstream inFile;
    std::string s;

    inFile.open(argv[3]);
    if (!inFile) {
        std::cerr << "Unable to open file";
        exit(1);
    }

    while (std::getline(inFile, s)) {
        std::vector<std::string> results;
        boost::split(results, s, [](char c) { return c == ' '; });

        if (results.size() == 3)
            port = std::stoi(results[2]);
        else
            port = client::default_port;
        if (ss.open(server_ip, port)) {

            std::string type = filesys::get_content_type(std::string(results[1]));
            if (!boost::to_upper_copy<std::string>(results[0]).compare("GET"))
                request::GET(results[1], client::http_version_1_0, client::user_agent, server_ip, false,
                             type, ss);
            else
                request::POST(results[1], client::http_version_1_0, client::user_agent, server_ip, ss);

            (*callbackMap[results[0]])(results, ss);
        } else {
            std::cerr << "Failed to connect to the server" << std::endl;
        }
    }
}

void client::receive_thread(std::vector<std::string> requests, net::socketstream &ss) {
    for (int i = 0; i < requests.size(); i++) {
        std::vector<std::string> results;
        boost::split(results, requests[i], [](char c) { return c == ' '; });
        sem.wait();
        (*callbackMap[results[0]])(results, ss);
    }
}

void client::http1_1(const char *argv[]) {
    int port = std::stoi(argv[2]);
    std::string server_ip = argv[1];
    std::ifstream inFile;
    client::net::socketstream ss;
    std::string s;

    inFile.open(argv[3]);
    if (!inFile) {
        std::cerr << "Unable to open file";
        exit(1);
    }

    if (ss.open(server_ip, port)) {
        std::vector<std::string> requests;
        while (std::getline(inFile, s))
            requests.push_back(s);
        // Start receiving thread
        std::thread receiving(&receive_thread, requests, std::ref(ss));
        for (int i = 0; i < requests.size(); i++) {
            std::vector<std::string> results;
            boost::split(results, requests[i], [](char c) { return c == ' '; });
            if (i != requests.size() - 1)
                request::GET(results[1], client::http_version_1_1, client::user_agent, server_ip, true,
                             filesys::get_content_type(results[1]), ss);
            else
                request::GET(results[1], client::http_version_1_1, client::user_agent, server_ip, false,
                             filesys::get_content_type(results[1]), ss);
            sem.notify();
        }
        receiving.join();
    } else {
        std::cerr << "Failed to connect to the server" << std::endl;
    }

}

