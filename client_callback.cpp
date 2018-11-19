//
// Created by heshamovic on 11/16/18.
//

#include "client_callback.h"
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>

void GET_callback(const std::vector<std::string> &results, client::net::socketstream &ss);

void POST_callback(const std::vector<std::string> &results,  client::net::socketstream &ss);

std::map<std::string, cfunc> callbackMap;

void callback::init() {
    callbackMap["GET"] = GET_callback;
    callbackMap["POST"] = POST_callback;
}


void GET_callback(const std::vector<std::string> &results, client::net::socketstream &ss) {
    char c;
    std::ostringstream oss;
    std::istringstream iss, riss;
    std::string response_status, http_version, status_msg, data;
    int status;

    while (ss.get(c))
        oss.put(c);

    iss.str(oss.str());
    std::getline(iss, response_status);
    riss.str(response_status);
    riss >> http_version >> status;
    status_msg = riss.str().substr(riss.tellg());

    // Skip headers
    while(std::getline(iss, data) && data.compare("\r"));

    if (status == client::ok_status) {
        data = iss.str().substr(iss.tellg());
        std::ofstream out(results[1]);
        std::cout << data;
        out << data;
        out.close();
    } else {
        std::cerr << status << status_msg;
    }

}

void POST_callback(const std::vector<std::string> &results, client::net::socketstream &ss) {
    char c;
    std::ostringstream oss;
    std::istringstream iss, riss;
    std::string response_status, http_version, status_msg;
    int status;

    while (ss.get(c))
        oss.put(c);

    iss.str(oss.str());
    std::getline(iss, response_status);
    riss.str(response_status);
    riss >> http_version >> status;
    status_msg = riss.str().substr(riss.tellg());

    if (status == client::ok_status) {
        std::cout << status << status_msg;
    } else {
        std::cerr << status << status_msg;
    }
}