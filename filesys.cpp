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
    std::cout << data << std::endl;
    return data;
}

int filesys::read_bytes(std::string path, void *data, int len) {
    FILE *fp = fopen(path.c_str(), "rb");
    int read = fread(data, 1, len, fp);
    fclose(fp);
    return read;
}

int filesys::write_bytes(std::string path, void *data, int len) {
    FILE *fp = fopen(path.c_str(), "wb+");
    int written = fwrite(data, 1, len, fp);
    fclose(fp);
    return written;
}
