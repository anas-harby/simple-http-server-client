#ifndef SIMPLE_HTTP_SERVER_CLIENT_FILESYS_H
#define SIMPLE_HTTP_SERVER_CLIENT_FILESYS_H

#include <string>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 
#include <iostream>

namespace filesys {
    bool exists(std::string path);
    long filesize(std::string path);
    time_t last_modified(std::string path);
    
    std::string read(std::string path);
    int write(std::string path, const std::string &data, int len = -1);
};

#endif
