//
// Created by awalid on 11/17/18.
//

#include <vector>
#include "parser.h"
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include "filesys.h"

std::string time_to_string(time_t t);
std::string get_content_type(std::string file_name);

http_request parser::parse(std::string req_str) {
    http_request http_req;
    std::vector<std::string> request_lines, req_line_fields;
    // Alternative splitting, which is correct?
    typedef boost::char_separator<char> separator;
    boost::tokenizer<separator> tokens(req_str, separator("\r\n"));
    std::copy(tokens.begin(), tokens.end(), std::back_inserter(request_lines));

    // Parsing request line
    std::string req_line = request_lines[0];
    boost::split(req_line_fields, req_line, [](char c) { return c == ' '; });
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

http_response get_response_get(http_request req) {
    http_response http_res;
    std::string file_path = req.get_file_path();

    if (!filesys::exists(file_path))
        http_res.set_status_code(http_response::status::NOT_FOUND);
    else {
        http_res.set_body(filesys::read(file_path));
        http_res.set_status_code(http_response::status::OK);
        
        http_res.add_header("Last-Modified", time_to_string(filesys::last_modified(file_path)));
        http_res.add_header("Content-Length", std::to_string((int) filesys::filesize(file_path)));
        http_res.add_header("Content-Type", get_content_type(file_path));
        http_res.add_header("Connection", "Closed");
    }
    
    time_t now; time(&now);
    http_res.add_header("Date", time_to_string(now));
    http_res.add_header("Server", "Simpleton-Server/1.0.0");

    http_res.set_version(req.get_version());
    return http_res;
}

http_response get_response_post(http_request req) {
    http_response http_res;
    int succ = 0;

    if (req.get_headers().find("Content-Length") == req.get_headers().end())
        succ = filesys::write(req.get_file_path(), req.get_data());
    else
        succ = filesys::write(req.get_file_path(), req.get_data(),
            std::stoi(req.get_headers().find("Content-Length")->second));

    if (succ != -1) {
        http_res.set_status_code(http_response::status::OK);
        http_res.add_header("Last-Modified", time_to_string(filesys::last_modified(req.get_file_path())));
        http_res.add_header("Content-Length", std::to_string((int) filesys::filesize(req.get_file_path())));
    } else
        http_res.set_status_code(http_response::status::BAD_REQUEST);

    time_t now; time(&now);
    http_res.add_header("Date", time_to_string(now));
    http_res.add_header("Server", "Simpleton-Server/1.0.0");

    http_res.set_version(req.get_version());
    return http_res;
}

http_response parser::get_response(http_request req) {
    if (req.get_type() == http_request::request_type::GET)
        return get_response_get(req);
    else if (req.get_type() == http_request::request_type::POST)
        return get_response_post(req);
    
    perror("Unsupported request type");
    return http_response();
}

http_response parser::get_timeout_response() {
    http_response http_res;
    http_res.set_status_code(http_response::status::REQUEST_TIMEOUT);
    return http_res;
}

std::string get_content_type(std::string file_name) {
    std::string extension = file_name.substr(file_name.find_last_of(".") + 1);
    if (extension.compare("html") == 0)
        return "text/html";
    else if (extension.compare("htm") == 0)
        return "text/htm";
    else if (extension.compare("txt") == 0)
        return "text/txt";
    else if (extension.compare("css") == 0)
        return "text/css";
    else if (extension.compare("js") == 0)
        return "text/javascript";
    else if (extension.compare("jpg") == 0)
        return "image/jpg";
    else if (extension.compare("jpeg") == 0)
        return "image/jpeg";
    else if (extension.compare("png") == 0)
        return "image/png";
    else if (extension.compare("gif") == 0)
        return "image/gif";
    return "unknown/unknown";
}

std::string time_to_string(time_t t) {
    char *c_date = std::ctime(&t);
    c_date[strlen(c_date) - 1] = '\0';
    return std::string(c_date);
}