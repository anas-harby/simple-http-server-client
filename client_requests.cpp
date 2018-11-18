//
// Created by heshamovic on 11/16/18.
//

#include "client_requests.h"
#include "client.h"
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

void GET(const std::vector<std::string> &results, client::net::socketstream &ss);

void POST(const std::vector<std::string> &results, client::net::socketstream &ss);

std::map<std::string, pfunc> requestMap;

void request::init() {
    requestMap["GET"] = GET;
    requestMap["POST"] = POST;
}


void GET(const std::vector<std::string> &results, client::net::socketstream &ss) {
    std::stringstream iss;
    iss << "GET " << results[1] << " " << client::http_version << "\r\n";
    iss << "Host: " << results[2] << "\r\n";
    iss << "Connection: close" << "\r\n";
    iss << "Accept: text/html" << "\r\n";
    iss << "\r\n";
    ss << iss.str() << std::flush;
}

void POST(const std::vector<std::string> &results, client::net::socketstream &ss) {
    std::stringstream iss;
    std::ifstream inFile(results[1]);

    if (!inFile) {
        std::cerr << "Unable to open file: " << results[1];
        return;
    }

    std::string data((std::istreambuf_iterator<char>(inFile)),
                    std::istreambuf_iterator<char>());


    iss << "POST " << results[1] << client::http_version << "\r\n";
    iss << "Host: " << results[2] << "\r\n";
    iss << "Content-Type: application/x-www-form-urlencoded" << "\r\n";
    iss << "Content-Length: " << data.length() << "\r\n";
    iss << "\r\n";
    iss << data << "\r\n";
    iss << "\r\n";
    ss << iss.str() << std::flush;
    inFile.close();
}
