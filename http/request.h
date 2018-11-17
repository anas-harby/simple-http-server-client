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
    request_type get_request_type();
    friend std::ostream& operator <<(std::ostream &strm, const http_request &request);

private:
    std::map<std::string, std::string> headers;
    std::string request_line;
    std::string data;
};

#endif //SIMPLE_HTTP_SERVER_CLIENT_REQUEST_H
