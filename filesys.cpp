#include "filesys.h"

bool filesys::exists(std::string path) {
    return std::ifstream(path.c_str()).good();
}

long filesys::filesize(std::string path) {
    std::ifstream ifs(path.c_str());
    ifs.seekg(0, ifs.end);
    long size = ifs.tellg();
    ifs.close();
    return size;
}

time_t filesys::last_modified(std::string path) {
    struct stat attr;
    stat(path.c_str(), &attr);
    return attr.st_mtim.tv_sec;
}

std::string filesys::read(std::string path) {
    std::ifstream ifs(path);
    std::string data((std::istreambuf_iterator<char>(ifs)),
                     std::istreambuf_iterator<char>());
    ifs.close();
    return data;
}

int filesys::write(std::string path, const std::string &data, int len) {
    if (len == -1)
        len = data.size();
    if (path[0] == '/')
        path = path.substr(1);
        
    if (path.find("/") != std::string::npos) {
        std::string dir = path.substr(0, path.find_last_of("/"));
        struct stat info;
        if (stat(dir.c_str(), &info) != 0) // Directory does not exist.
            return -1;
    }
    // write contents
    FILE *fp = fopen(path.c_str(), "wb+");
    int written = fwrite(data.c_str(), 1, len, fp);
    std::fflush(fp);
    fclose(fp);
    return written;
}

std::string filesys::get_content_type(std::string file_name) {
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