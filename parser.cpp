//
// Created by awalid on 11/17/18.
//

#include <vector>
#include "parser.h"
#include <boost/algorithm/string.hpp>
#include <iostream>

http_request parser::parse(std::string ss) {
    http_request http_req;
    std::vector<std::string> request_lines, req_line_fields;
    boost::split(request_lines, ss, boost::is_any_of("\n"));

    // Parsing request line
    std::string req_line = request_lines[0];
    boost::split(req_line_fields, req_line, boost::is_any_of("\t "));
    if (req_line_fields[0] == "GET") {
        http_req.set_type(http_request::GET);
    } else if (req_line_fields[0] == "POST") {
        http_req.set_type(http_request::POST);
    }
    http_req.set_file_path(req_line_fields[1]);
    http_req.set_version(req_line_fields[2]);

    // Parsing header lines
    int line_num;
    for (line_num = 1; line_num < request_lines.size(); line_num++) {
        if (request_lines[line_num].empty()) {
            line_num++;
            break;
        }
        std::vector<std::string> header_fields;
        boost::split(header_fields, request_lines[line_num], boost::is_any_of(":"));
        http_req.add_header(header_fields[0], header_fields[1]);
    }

    // Parsing body lines
    std::string body = std::string();
    for (line_num; line_num < request_lines.size(); line_num++) {
        body.append(request_lines[line_num]);
        if (line_num != request_lines.size() - 1)
            body.append("\n");
    }
    http_req.set_data(body);
    return http_req;
}