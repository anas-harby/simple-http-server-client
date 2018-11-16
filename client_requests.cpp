//
// Created by heshamovic on 11/16/18.
//

#include "client_requests.h"
#include "client.h"
#include <vector>
#include <sstream>

std::string GET(std::vector<std::string>);

std::string POST(std::vector<std::string>);

std::map<std::string, pfunc> requestMap;

void request::init() {
    requestMap["GET"] = GET;
    requestMap["POST"] = POST;
}


std::string GET(std::vector<std::string> results) {
    std::stringstream ss;
    ss << "GET " << results[1] << client::http_version << "\r\n";
    ss << "Host: " << results[2] << "\r\n";
    ss << "Accept: text/html" << "\r\n";
    ss << "\r\n";
    return ss.str();
}

std::string POST(std::vector<std::string> results) {

}
