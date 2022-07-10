#pragma once
#include <queue>

class Client;

struct Operation {
    int ts = 0;
    int code = 0;
    int value = 0;
    int index = 0;
}

class Server {
 public:

    void push_operation(const Operation& operation) {
        ops_queue.push(operation);
        for (auto& client : clients) {
            client.push_operation(operation);
        }
    }

    void execute_operation() {
        if (ops_queue.empty()) {
            return;
        }

        int [ts, code, value, index] = ops_queue.top();

        if (code == 0) {
            data.insert(index - 1, value);
        } else if (code == 1) {
            data.erase(index);
        } else if (code == 2) {
            crdt_array.update(index, value);
        }

        ops_queue.pop();
    }

 private:
    CrdtArray data;
    std::vector<Client> clients;
    std::priority_queue<Operation> ops_queue;
};
