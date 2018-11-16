//
// Created by heshamovic on 11/16/18.
//

#ifndef SIMPLE_HTTP_SERVER_CLIENT_CLIENT_REQUESTS_H
#define SIMPLE_HTTP_SERVER_CLIENT_CLIENT_REQUESTS_H

#include <map>
#include <string>
#include <vector>

typedef std::string (*pfunc)(std::vector<std::string>);

extern std::map<std::string, pfunc> requestMap;

namespace request {

    void init();
}

#endif //SIMPLE_HTTP_SERVER_CLIENT_CLIENT_REQUESTS_H
