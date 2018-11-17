//
// Created by awalid on 11/17/18.
//

#include "working_thread.h"

template<typename _Callable, typename... _Args>
working_thread::working_thread(_Callable &&__f, _Args &&... __args) {
    working_thread::thread = new std::thread(__f, __args...);
    working_thread::done = false;
}

working_thread::working_thread(std::thread *th) {
    working_thread::thread = th;
    working_thread::done = false;
}

std::thread::id working_thread::get_thread_id() {
    return working_thread::thread->get_id();
}

void working_thread::detach() {
  working_thread::thread->detach();
}

void working_thread::mark_done() {
    working_thread::done = true;
}

bool working_thread::is_done() {
    return working_thread::done;
}

working_thread::~working_thread() {
    delete working_thread::thread;
}

