//
// Created by heshamovic on 11/16/18.
//

#ifndef SIMPLE_HTTP_SERVER_CLIENT_CLIENT_REQUESTS_H
#define SIMPLE_HTTP_SERVER_CLIENT_CLIENT_REQUESTS_H

#include <map>
#include <string>
#include <vector>
#include "client.h"

namespace request {
    void GET(const std::string file_name, const std::string http_version, const std::string user_agent,
                const std::string host, bool keep_alive, const std::string type, client::net::socketstream &ss);
    void POST(const std::string file_name, const std::string http_version, const std::string user_agent,
            const std::string host, client::net::socketstream &ss);
}

#endif //SIMPLE_HTTP_SERVER_CLIENT_CLIENT_REQUESTS_H
