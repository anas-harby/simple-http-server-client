#include "response.h"

http_response::http_response() {

}

const std::map<std::string, std::string> &http_response::get_headers() const {
    return headers;
}

void http_response::add_header(std::string key_header, std::string key_value) {
    http_response::headers[key_header] = std::move(key_value);   
}

http_response::response_type http_response::get_type() const {
    return http_response::type;
}

void http_response::set_type(http_response::response_type type) {
    http_response::type = type;
}

const std::string &http_response::get_body() const {
    return http_response::body;
}

void http_response::set_body(const std::string &body) {
    http_response::body = body;
}

int http_response::get_status_code() const {
    return http_response::status_code;
}

void http_response::set_status_code(const int s) {
    http_response::status_code = s;
}

std::string http_response::get_version() const {
    return http_response::version;
}

void http_response::set_version(const std::string version) {
    http_response::version = version;
}

std::ostream& operator <<(std::ostream &strm, const http_response &response) {
    strm << response.get_version() << " " << response.get_status_code() << " " << response.get_reason_phrase();
    strm << "\r\n";
    if (!response.get_headers().empty()) {
        for (auto header : response.get_headers())
            strm << header.first << ": " << header.second << "\r\n";
        strm << "\r\n";
    }
    strm << response.body;
    return strm;
}

