//
// Created by awalid on 11/17/18.
//

#ifndef SIMPLE_HTTP_SERVER_CLIENT_PARSER_H
#define SIMPLE_HTTP_SERVER_CLIENT_PARSER_H


#include "http/request.h"
#include "http/response.h"

namespace parser {
    http_request parse(std::string ss);

    http_response get_response(http_request req);
};


#endif //SIMPLE_HTTP_SERVER_CLIENT_PARSER_H
