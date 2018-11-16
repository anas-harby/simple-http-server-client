//
// Created by heshamovic on 11/16/18.
//

#include "client_callback.h"
#include <vector>
#include <sstream>
#include <iostream>

void GET(std::string);

void POST(std::string);

std::map<std::string, cfunc> callbackMap;

void callback::init() {
    callbackMap["GET"] = GET;
    callbackMap["POST"] = POST;
}


void GET(std::string results) {
    std::cout << results;
}

void POST(std::string results) {

}