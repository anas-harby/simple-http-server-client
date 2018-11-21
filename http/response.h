#ifndef SIMPLE_HTTP_SERVER_CLIENT_RESPONSE_H
#define SIMPLE_HTTP_SERVER_CLIENT_RESPONSE_H

#include <string>
#include <map>
#include <iostream>

class http_response {
public:
    enum response_type {
        GET,
        POST
    };

    enum status {
        OK = 200,
        CREATED = 201,
        BAD_REQUEST = 400,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        REQUEST_TIMEOUT = 408
    };

    inline std::string get_reason_phrase() const {
        switch(http_response::status_code) {
            case OK:
                return "OK";
            case CREATED:
                return "Created";
            case BAD_REQUEST:
                return "Bad Request";
            case FORBIDDEN:
                return "Forbidden";
            case NOT_FOUND:
                return "Not Found";
            case REQUEST_TIMEOUT:
                return "Request Timeout";
            default:
                return "Unknown Status";
        }
        return "Unknown Status";
    }

    http_response();
    response_type get_response_type();
    friend std::ostream& operator <<(std::ostream &strm, const http_response &response);

private:
    std::map<std::string, std::string> headers;
public:
    const std::map<std::string, std::string> &get_headers() const;
    void add_header(std::string key_header, std::string key_value);

    response_type get_type() const;
    void set_type(response_type type);

    const std::string &get_body() const;
    void set_body(const std::string &body);

    int get_status_code() const;
    void set_status_code(const int s);

private:
    response_type type;
    int status_code;

    std::string body;
};

#endif //SIMPLE_HTTP_SERVER_CLIENT_RESPONSE_H
