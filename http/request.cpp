//
// Created by awalid on 11/17/18.
//

#include "request.h"

http_request::http_request() {

}

http_request::request_type http_request::get_request_type() {
    return GET;
}

std::ostream &operator <<(std::ostream &strm, const http_request &request) {
    return strm;
}
