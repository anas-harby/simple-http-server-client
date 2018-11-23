//
// Created by heshamovic on 11/16/18.
//

#include "client_requests.h"
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>


void request::GET(const std::string file_name, const std::string http_version, const std::string user_agent,
        const std::string host, bool keep_alive, const std::string type, client::net::socketstream &ss) {
    std::stringstream iss;

    iss << "GET " << file_name << " " << http_version << "\r\n";
    iss << "User-Agent: " << user_agent << "\r\n";
    iss << "Host: " << host << "\r\n";
    iss << "Accept-Language: en-us\r\n";
    if (keep_alive)
        iss << "Connection: keep-alive" << "\r\n";
    else
        iss << "Connection: close" << "\r\n";
    iss << "Accept: " << type << "\r\n";
    iss << "\r\n";

    ss << iss.str() << std::flush;
}

void request::POST(const std::string file_name, const std::string http_version, const std::string user_agent,
        const std::string host, client::net::socketstream &ss) {
    std::stringstream iss;
    std::ifstream inFile(file_name);

    if (!inFile) {
        std::cerr << "Unable to open file: " << file_name;
        return;
    }

    std::string data((std::istreambuf_iterator<char>(inFile)),
                    std::istreambuf_iterator<char>());

    iss << "POST " << file_name << " " << http_version << "\r\n";
    iss << "Host: " << host << "\r\n";
    iss << "Content-Type: application/x-www-form-urlencoded" << "\r\n";
    iss << "Content-Length: " << data.length() << "\r\n";
    iss << "\r\n";
    iss << data;
    ss << iss.str() << std::flush;
    inFile.close();
}
