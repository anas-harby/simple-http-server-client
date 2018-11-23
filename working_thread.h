#ifndef SIMPLE_HTTP_SERVER_CLIENT_WORKING_THREAD_H
#define SIMPLE_HTTP_SERVER_CLIENT_WORKING_THREAD_H

#include <time.h>
#include <thread>

class working_thread {
public:
    template<typename _Callable, typename... _Args>
    working_thread(_Callable &&__f, _Args &&... __args);
    working_thread(std::thread *th);

    void detach();
    void kill();
    void mark_done();
    bool is_done();
    std::thread::id get_thread_id();
    time_t get_last_wait_time();
    virtual ~working_thread();

private:
    std::thread *thread;
    bool done;
    time_t last_wait_time;
};


#endif //SIMPLE_HTTP_SERVER_CLIENT_WORKING_THREAD_H
