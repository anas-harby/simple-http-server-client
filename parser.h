//
// Created by awalid on 11/17/18.
//

#ifndef SIMPLE_HTTP_SERVER_CLIENT_PARSER_H
#define SIMPLE_HTTP_SERVER_CLIENT_PARSER_H


#include "http/request.h"

namespace parser {
    http_request parse(std::string ss);
};


#endif //SIMPLE_HTTP_SERVER_CLIENT_PARSER_H
