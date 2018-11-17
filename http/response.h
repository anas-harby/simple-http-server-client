//
// Created by awalid on 11/17/18.
//

#ifndef SIMPLE_HTTP_SERVER_CLIENT_RESPONSE_H
#define SIMPLE_HTTP_SERVER_CLIENT_RESPONSE_H

#include <string>

namespace http_response {
    class http_get_response {
    public:
        http_get_response();
        std::ostream&operator <<(std::ostream &strm, const http_get_response &response);

    private:
        std::string file_name;
        std::string host;
        std::string type;
        std::string accept;
    };

    class http_post_response {
    public:
        http_post_response();
        std::ostream&operator <<(std::ostream &strm, const http_post_response &response);

    private:

    };
};


#endif //SIMPLE_HTTP_SERVER_CLIENT_REQUEST_H



#endif //SIMPLE_HTTP_SERVER_CLIENT_RESPONSE_H
