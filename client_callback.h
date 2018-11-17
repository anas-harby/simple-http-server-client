//
// Created by heshamovic on 11/16/18.
//

#ifndef SIMPLE_HTTP_SERVER_CLIENT_CLIENT_CALLBACK_H
#define SIMPLE_HTTP_SERVER_CLIENT_CLIENT_CALLBACK_H


#include <map>
#include <string>
#include <vector>
#include "client.h"

typedef void (*cfunc)(const std::vector<std::string> &results, client::net::socketstream &ss);

extern std::map<std::string, cfunc> callbackMap;

namespace callback {

    void init();
}
#endif //SIMPLE_HTTP_SERVER_CLIENT_CLIENT_CALLBACK_H
