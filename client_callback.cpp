//
// Created by heshamovic on 11/16/18.
//

#include "client_callback.h"
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <string>
#include "http/response.h"

void GET_callback(const std::vector<std::string> &results, client::net::socketstream &ss);

void POST_callback(const std::vector<std::string> &results,  client::net::socketstream &ss);

std::string read_response(client::net::socketstream &ss);

std::map<std::string, cfunc> callbackMap;

void callback::init() {
    callbackMap["GET"] = GET_callback;
    callbackMap["POST"] = POST_callback;
}


void GET_callback(const std::vector<std::string> &results, client::net::socketstream &ss) {
    std::ostringstream oss;
    std::istringstream iss, riss;
    std::string response_status, http_version, status_msg, data;
    int status;

    std::string response = read_response(ss);
    iss.str(response);

    std::getline(iss, response_status);
    riss.str(response_status);
    riss >> http_version >> status;
    status_msg = riss.str().substr(riss.tellg());

    if (status == http_response::status::OK){
        //    // Skip headers
        while(std::getline(iss, data) && data.compare("\r"));
        data = iss.str().substr(iss.tellg());
        std::ofstream out(results[1]);
        std::cout << data << std::endl;
        out << data;
        out.close();
    }
    else {
        std::cerr << status << " " << status_msg << std::endl;
    }
    std::cout << response << std::endl;

}

void POST_callback(const std::vector<std::string> &results, client::net::socketstream &ss) {
    std::ostringstream oss;
    std::istringstream iss, riss;
    std::string response_status, http_version, status_msg, data;
    int status;

    std::string response = read_response(ss);
    iss.str(response);

    std::getline(iss, response_status);
    riss.str(response_status);
    riss >> http_version >> status;
    status_msg = riss.str().substr(riss.tellg());

    if (status == client::ok_status) {
        std::cout << status << " " << status_msg << std::endl;
    } else {
        std::cerr << status << " " << status_msg << std::endl;
    }
    std::cout << response << std::endl;
}

std::string read_response(client::net::socketstream &ss){
    char c;
    std::ostringstream oss;
    int len;
    std::string line;
    // Read till content length
    while(std::getline(ss, line)){
        oss << line << "\n";
            if(!line.find("Content-Length")){
                std::vector<std::string> results;
                boost::split(results, line, [](char c) { return c == ' '; });
                len = std::stoi(results.back());
            }
            else if(!line.find("\r"))
                break;
    }
    for(int i = 0; i<len; i++){
        ss.get(c);
        oss.put(c);
    }
    return oss.str();
}