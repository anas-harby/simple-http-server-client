//
// Created by heshamovic on 11/23/18.
//

#ifndef SIMPLE_HTTP_SERVER_CLIENT_UTIL_H
#define SIMPLE_HTTP_SERVER_CLIENT_UTIL_H
#include <mutex>
#include <condition_variable>

class Semaphore {
public:
    Semaphore (int count_ = 0)
            : count(count_) {}

    inline void notify()
    {
        std::unique_lock<std::mutex> lock(mtx);
        count++;
        cv.notify_one();
    }

    inline void wait()
    {
        std::unique_lock<std::mutex> lock(mtx);

        while(count == 0){
            cv.wait(lock);
        }
        count--;
    }

private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;
};
#endif //SIMPLE_HTTP_SERVER_CLIENT_UTIL_H
