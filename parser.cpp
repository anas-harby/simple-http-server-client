//
// Created by awalid on 11/17/18.
//

#include <vector>
#include "parser.h"
#include <boost/algorithm/string.hpp>
#include <iostream>

http_request parser::parse(std::string ss) {
    std::vector<std::string> request_lines;
    boost::split(request_lines, ss, boost::is_any_of("\n"));
    for (auto line : request_lines) {
        std::cout << line << std::endl;
    }
    return http_request();
}