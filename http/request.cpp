//
// Created by awalid on 11/17/18.
//

#include "request.h"

#include <utility>
#include <iostream>

http_request::http_request() {}

std::ostream &operator <<(std::ostream &strm, const http_request &request) {
    strm << request.get_request_type_string() << " " << request.get_file_path() << " " << request.get_version() << "\r\n";
    for (auto header : request.get_headers()) {
        strm << header.first << ": " << header.second << "\r\n";
    }
    strm << "\r\n";
    if (!request.get_data().empty()) {
        strm << request.get_data() << "\r\n";
    }
    return strm;
}

const std::map<std::string, std::string> &http_request::get_headers() const {
    return headers;
}

void http_request::add_header(std::string header_key, std::string header_value) {
    http_request::headers[header_key] = std::move(header_value);
}

http_request::request_type http_request::get_type() const {
    return type;
}

void http_request::set_type(http_request::request_type type) {
    http_request::type = type;
}

const std::string &http_request::get_file_path() const {
    return file_path;
}

void http_request::set_file_path(const std::string &file_path) {
    http_request::file_path = file_path;
}

const std::string &http_request::get_version() const {
    return version;
}

void http_request::set_version(const std::string &version) {
    http_request::version = version;
}

const std::string &http_request::get_data() const {
    return data;
}

void http_request::set_data(const std::string &data) {
    http_request::data = data;
}
