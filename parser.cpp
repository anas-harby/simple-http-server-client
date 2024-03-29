//
// Created by awalid on 11/17/18.
//

#include <vector>
#include "parser.h"
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <sys/socket.h>
#include "filesys.h"

std::string time_to_string(time_t t);

std::string read_req_from_socket(const int socket, size_t buf_size) {
    char buffer[buf_size + 1] = {0};
    ssize_t value_read = recv(socket, buffer, buf_size, 0);
    buffer[buf_size] = '\0';
    if (value_read == -1) {
        std::cerr << "Failed to read from socket" << std::endl;
        exit(EXIT_FAILURE);
    }
    return std::string(buffer);
}

void parse_headers(std::string &req_str, http_request &http_req) {
    // Splitting the string
    std::vector<std::string> lines;
    boost::split(lines, req_str, boost::is_any_of("\n"));

    // Parsing request line
    std::vector<std::string> req_line_fields;
    std::string req_line = lines[0];
    boost::split(req_line_fields, req_line, [](char c) { return c == ' '; });
    if (req_line_fields[0] == "GET") {
        http_req.set_type(http_request::GET);
    } else if (req_line_fields[0] == "POST") {
        http_req.set_type(http_request::POST);
    }
    boost::trim(req_line_fields[1]);
    boost::trim(req_line_fields[2]);
    http_req.set_file_path(req_line_fields[1]);
    http_req.set_version(req_line_fields[2]);

    // Parsing headers
    for (int i = 1; i < lines.size(); i++) {
        std::vector<std::string> header_fields;
        boost::split(header_fields, lines[i], boost::is_any_of(":"));
        boost::trim(header_fields[0]);
        boost::trim(header_fields[1]);
        http_req.add_header(header_fields[0], header_fields[1]);
    }
}

void parse_body(http_request &http_req, std::string body_str, int socket) {
    std::string body_chunk_read = body_str;
    http_req.append_to_body(body_chunk_read);
    // Check if content-length isn't found in the headers, return error.
    size_t content_length = static_cast<size_t>(std::stoi(http_req.get_header_value("Content-Length")));
    while (content_length > http_req.get_body().length()) {
        std::string body_chunk = read_req_from_socket(socket, parser::CHUNK_SIZE);
        http_req.append_to_body(body_chunk);
    }
}

std::vector<http_request> parser::parse(const int socket, std::string &rest) {
    std::vector<http_request> requests;

    std::string request_str = read_req_from_socket(socket, parser::MAX_HEADERS_SIZE);
    std::string rest_str = rest + request_str;

    size_t delim_pos = rest_str.find("\r\n\r\n");
    while (delim_pos != std::string::npos) {
        http_request http_req;

        std::string headers_str = rest_str.substr(0, delim_pos + 1);
        parse_headers(headers_str, http_req);
        rest_str = rest_str.substr(delim_pos + 4);
        if (http_req.get_type() == http_request::POST) {
            parse_body(http_req, rest_str, socket);
            requests.push_back(http_req);
            rest_str = std::string();
            break;
        }
        requests.push_back(http_req);
        delim_pos = rest_str.find("\r\n\r\n");
    }
    rest = rest_str;
    return requests;
}


http_response get_response_get(http_request req) {
    http_response http_res;
    std::string file_path = req.get_file_path();

    if (!filesys::exists(file_path)) {
        http_res.set_status_code(http_response::status::NOT_FOUND);
        http_res.add_header("Content-Length", "0");
    }
    else {
        http_res.set_body(filesys::read(file_path));
        http_res.set_status_code(http_response::status::OK);
        
        http_res.add_header("Last-Modified", time_to_string(filesys::last_modified(file_path)));
        http_res.add_header("Content-Length", std::to_string((int) filesys::filesize(file_path)));
        http_res.add_header("Content-Type", filesys::get_content_type(file_path));
        http_res.add_header("Connection", req.header_exists("Connection")
                && req.get_header_value("Connection") == "keep-alive" ? "Open" : "Close");
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
        succ = filesys::write(req.get_file_path(), req.get_body());
    else
        succ = filesys::write(req.get_file_path(), req.get_body(),
            std::stoi(req.get_headers().find("Content-Length")->second));

    if (succ != -1) {
        http_res.set_status_code(http_response::status::OK);
        http_res.add_header("Last-Modified", time_to_string(filesys::last_modified(req.get_file_path())));
    } else
        http_res.set_status_code(http_response::status::BAD_REQUEST);

    time_t now; time(&now);
    http_res.add_header("Date", time_to_string(now));
    http_res.add_header("Content-Length", "0");
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
    http_res.set_version("HTTP/1.1");
    return http_res;
}

std::string time_to_string(time_t t) {
    char *c_date = std::ctime(&t);
    c_date[strlen(c_date) - 1] = '\0';
    return std::string(c_date);
}