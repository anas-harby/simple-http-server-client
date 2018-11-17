//
// Created by heshamovic on 11/16/18.
//

#include "client_requests.h"
#include "client.h"
#include <vector>
#include <sstream>

void GET(const std::vector<std::string> &results, client::net::socketstream &ss);

void POST(const std::vector<std::string> &results, client::net::socketstream &ss);

std::map<std::string, pfunc> requestMap;

void request::init() {
    requestMap["GET"] = GET;
    requestMap["POST"] = POST;
}


void GET(const std::vector<std::string> &results, client::net::socketstream &ss) {
    std::stringstream iss;
    iss << "GET " << results[1] << client::http_version << "\r\n";
    iss << "Host: " << results[2] << "\r\n";
    iss << "Accept: text/html" << "\r\n";
    iss << "\r\n";
    ss << iss.str() << std::flush;
}

void POST(const std::vector<std::string> &results, client::net::socketstream &ss) {

}
