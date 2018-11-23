//
// Created by awalid on 11/17/18.
//

#include "working_thread.h"

template<typename _Callable, typename... _Args>
working_thread::working_thread(_Callable &&__f, _Args &&... __args) {
    working_thread::thread = new std::thread(__f, __args...);
    working_thread::done = false;
    working_thread::last_wait_time = 0L;
}

working_thread::working_thread(std::thread *th) {
    working_thread::thread = th;
    working_thread::done = false;
    working_thread::last_wait_time = 0L;
}

std::thread::id working_thread::get_thread_id() {
    return working_thread::thread->get_id();
}

void working_thread::detach() {
    std::time(&(working_thread::last_wait_time));
    working_thread::thread->detach();
}

void working_thread::kill() {
    pthread_cancel(working_thread::thread->native_handle());
}

void working_thread::mark_done() {
    working_thread::done = true;
}

bool working_thread::is_done() {
    return working_thread::done;
}

time_t working_thread::get_last_wait_time() {
    return working_thread::last_wait_time;
}

working_thread::~working_thread() {
    delete working_thread::thread;
}

