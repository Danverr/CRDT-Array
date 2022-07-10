#pragma once

#include "Server.h"

class Client {
 public:
    Client(std::shared_ptr<Server> server) : server_(server) {
    }

    start() {

    }

 private:
    std::shared_ptr<Server> server_;

};