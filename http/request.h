//
// Created by awalid on 11/17/18.
//

#ifndef SIMPLE_HTTP_SERVER_CLIENT_REQUEST_H
#define SIMPLE_HTTP_SERVER_CLIENT_REQUEST_H

#include <string>
#include <map>

class http_request {
public:
    enum request_type {
        GET,
        POST
    };

    http_request();
    friend std::ostream& operator <<(std::ostream &strm, const http_request &request);

private:
    std::map<std::string, std::string> headers;
public:
    const std::map<std::string, std::string> &get_headers() const;

    void add_header(std::string key_header, std::string key_value);

    request_type get_type() const;

    void set_type(request_type type);

    const std::string &get_file_path() const;

    void set_file_path(const std::string &file_path);

    const std::string &get_version() const;

    void set_version(const std::string &version);

    const std::string &get_data() const;

    void set_data(const std::string &data);

private:
    request_type type;
    std::string file_path, version;
    std::string data;
};

#endif //SIMPLE_HTTP_SERVER_CLIENT_REQUEST_H