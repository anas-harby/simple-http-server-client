//
// Created by awalid on 11/17/18.
//

#ifndef SIMPLE_HTTP_SERVER_CLIENT_REQUEST_H
#define SIMPLE_HTTP_SERVER_CLIENT_REQUEST_H

#include <string>
#include <map>
#include <vector>

class http_request {
public:
    enum request_type {
        GET,
        POST
    };

    inline std::string get_request_type_string() const {
        if (http_request::type == http_request::GET) {
            return "GET";
        } else if (http_request::type == http_request::POST) {
            return "POST";
        } else {
            return "UNKNOWN";
        }
    }

    http_request();

    friend std::ostream& operator <<(std::ostream &strm, const http_request &request);

    const std::map<std::string, std::string> &get_headers() const;

    void add_header(std::string key_header, std::string key_value);

    request_type get_type() const;

    void set_type(request_type type);

    const std::string &get_file_path() const;

    void set_file_path(const std::string &file_path);

    const std::string &get_version() const;

    void set_version(const std::string &version);

    const std::string &get_body() const;

    void append_to_body(const std::string &s);

    const std::string &get_header_value(const std::string &key);

    bool header_exists(const std::string &s);

private:
    request_type type;
    std::string file_path, version;
    std::map<std::string, std::string> headers;
    std::string body;
};

#endif //SIMPLE_HTTP_SERVER_CLIENT_REQUEST_H