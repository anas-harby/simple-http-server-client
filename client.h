//
// Created by heshamovic on 11/16/18.
//

#ifndef SIMPLE_HTTP_SERVER_CLIENT_CLIENT_H
#define SIMPLE_HTTP_SERVER_CLIENT_CLIENT_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <streambuf>
#include <istream>
#include <ostream>
#include <unistd.h>

namespace client {
    const std::string http_version = "HTTP/1.0";
    const int ok_status = 200;
    const int default_port = 80;

    namespace net {

        template<typename Char>
        class basic_socketbuf
                : public std::basic_streambuf<Char> {
        public:
            typedef Char char_type;
            typedef std::basic_streambuf<char_type> buf_type;
            typedef std::basic_ostream<char_type> stream_type;
            typedef typename buf_type::int_type int_type;
            typedef typename std::basic_streambuf<Char>::traits_type traits_type;

        protected:

            static const int char_size = sizeof(char_type);
            static const int SIZE = 3145728;
            char_type obuf[SIZE];
            char_type ibuf[SIZE];

            int sock;

        public:
            basic_socketbuf();

            virtual ~basic_socketbuf();

            void set_socket(int sock);

            int get_socket();

        protected:

            int output_buffer();

            virtual int_type overflow(int_type c) {
                if (c != traits_type::eof()) {
                    *buf_type::pptr() = c;
                    buf_type::pbump(1);
                }

                if (output_buffer() == traits_type::eof())
                    return traits_type::eof();
                return c;
            }

            virtual int sync();

            virtual int_type underflow() {
                if (buf_type::gptr() < buf_type::egptr())
                    return *buf_type::gptr();

                int num;
                if ((num = recv(sock, reinterpret_cast<char *>(ibuf), SIZE * char_size, 0)) <= 0)
                    return traits_type::eof();

                buf_type::setg(ibuf, ibuf, ibuf + num);
                return *buf_type::gptr();
            }
        };

        typedef basic_socketbuf<char> socketbuf;
        typedef basic_socketbuf<wchar_t> wsocketbuf;

        template<typename Char>
        class basic_socketstream
                : public std::basic_iostream<Char> {
        public:
            typedef Char char_type;
            typedef std::basic_iostream<char_type> stream_type;
            typedef basic_socketbuf<char_type> buf_type;

        protected:
            buf_type buf;

        public:
            basic_socketstream();

            basic_socketstream(int s);

            void close_socket();

            bool open(const std::string &host, uint16_t port);
        };

        typedef basic_socketstream<char> socketstream;
        typedef basic_socketstream<wchar_t> wsocketstream;

    }
}

#endif //SIMPLE_HTTP_SERVER_CLIENT_CLIENT_H
