//
// Created by awalid on 11/17/18.
//

#ifndef SIMPLE_HTTP_SERVER_CLIENT_PARSER_H
#define SIMPLE_HTTP_SERVER_CLIENT_PARSER_H


#include "http/request.h"
#include "http/response.h"

namespace parser {
    const int CHUNK_SIZE = 8192;
    const int MAX_HEADERS_SIZE = 16384;

    http_request parse(int socket);
    http_response get_response(http_request req);
    http_response get_timeout_response();
};


#endif //SIMPLE_HTTP_SERVER_CLIENT_PARSER_H
