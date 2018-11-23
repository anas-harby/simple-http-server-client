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
std::string get_content_type(std::string file_name);

void read_req_from_socket(const int socket, std::vector<std::string> &request_lines, size_t buf_size) {
    char buffer[buf_size];
    ssize_t value_read = recv(socket, buffer, buf_size, 0);
    if (value_read == -1) {
        std::cerr << "Failed to read from socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Splitting request into vector of strings(lines)
    request_lines.clear();
    std::string str = std::string(buffer);
    boost::split(request_lines, str, boost::is_any_of("\n"));
}

void parse_req_line(std::vector<std::string> &request_lines, int &line_no, http_request &http_req) {
    std::vector<std::string> req_line_fields;
    std::string req_line = request_lines[line_no++];
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
}

bool parse_headers(std::vector<std::string> &request_lines, int &line_no, http_request &http_req) {
    bool found_delim = false;
    for (line_no; line_no < request_lines.size(); line_no++) {

        if (request_lines[line_no] == "\r") {
            found_delim = true;
            line_no++;
            break;
        }
        std::vector<std::string> header_fields;
        boost::split(header_fields, request_lines[line_no], boost::is_any_of(":"));
        boost::trim(header_fields[0]);
        boost::trim(header_fields[1]);
        http_req.add_header(header_fields[0], header_fields[1]);
    }
    return found_delim;
}

void parse_body(std::vector<std::string> &request_lines, int &line_no, http_request &http_req) {
    for (line_no; line_no < request_lines.size(); line_no++) {
        http_req.append_to_body(request_lines[line_no]);
        if (line_no != request_lines.size() - 1)
            http_req.append_to_body("\n");
    }
}

http_request parser::parse(const int socket) {
    http_request http_req;
    std::vector<std::string> request_lines;
    read_req_from_socket(socket, request_lines, parser::BUFF_SIZE);

    int line_no = 0;
    parse_req_line(request_lines, line_no, http_req);

    while (!parse_headers(request_lines, line_no, http_req)) {
        read_req_from_socket(socket, request_lines, parser::BUFF_SIZE);
        line_no = 0;
    }

    if (http_req.get_type() == http_request::POST) {
        parse_body(request_lines, line_no, http_req); // Append rest of buffer to body
        size_t content_length = static_cast<size_t>(std::stoi(http_req.get_header_value("Content-Length")));
        if (content_length > http_req.get_body().length()) {
            read_req_from_socket(socket, request_lines, content_length);
            line_no = 0;
            parse_body(request_lines, line_no, http_req);
        }
    }
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
    
    return http_res;
}

http_response get_response_post(http_request req) {
    http_response http_res;
    int succ = 0;    
    if (req.get_headers().find("Content-Length")
            == req.get_headers().end())
        succ = filesys::write(req.get_file_path(), req.get_body());
    else
        succ = filesys::write(req.get_file_path(), req.get_body(),
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

    return http_res;
}


http_response parser::get_response(http_request req) {
    if (req.get_type() == http_request::request_type::GET)
        return get_response_get(req);
    else if (req.get_type() == http_request::request_type::POST)
        return get_response_post(req);
    else
        perror("Unsupported request type");
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