#pragma once
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <ext/rope>
#include <vector>
#include <queue>
#include <deque>
#include <unordered_map>
#include <memory>
using namespace __gnu_pbds;
using namespace __gnu_cxx;

template <typename T>
using ordered_set = tree<T, null_type, std::less<T>, rb_tree_tag, tree_order_statistics_node_update>;

class CrdtArray {

 public:
    explicit CrdtArray(const std::vector<int>& values) {
        int size_left = values.size();
        int index = 0;
        std::queue<std::shared_ptr<Node>> nodes_queue({root_});
        build_tree(nodes_queue, size_left);
        fill_tree(values, index, root_);
    }

    void insert(int index, int value) {
        if (index == -1) {
            insert(root_, value);
        } else {
            insert(nodes_[get_id(index)], value);
        }
    }

    void erase(int index) {
        delete_node(get_id(index));
    }

    void update(int index, int value) {
        insert(index, value);
        erase(index);
    }

    int operator[](int index) {
        return nodes_[get_id(index)]->value;
    }

    size_t size() const {
        return ids_.size();
    }

 private:

    struct Node {
        std::string id;
        int value = 0;
        std::deque<std::shared_ptr<Node>> children = {};
    };

    const int MAX_NODE_SIZE = 3; // [1; 94]
    const char MAX_CHAR = char(126);
    std::shared_ptr<Node> root_ = std::make_shared<Node>();
    std::unordered_map<std::string, std::shared_ptr<Node>> nodes_;
    ordered_set<std::string> ids_;

    void insert(const std::shared_ptr<Node>& node, const int& value) {
        if (node->children.size() < MAX_NODE_SIZE) {
            create_child(node, value);
        } else {
            std::shared_ptr<Node> next_child = node->children.front();

            if (nodes_.contains(next_child->id)) {
                int next_value = next_child->value;
                next_child->value = value;
                insert(next_child, next_value);
            } else {
                insert(next_child, value);
            }
        }
    }

    std::string get_next_child_id(const std::shared_ptr<Node>& parent) const {
        std::string id;

        if (!parent->children.empty()) {
            id = parent->children.front()->id;
            id[id.size() - 1] = char(int(id[id.size() - 1]) - 1);
        } else {
            id = parent->id + MAX_CHAR;
        }

        return id;
    }

    std::shared_ptr<Node> create_child(const std::shared_ptr<Node>& parent, int value = 0) {
        std::string id = get_next_child_id(parent);
        std::shared_ptr<Node> node = std::make_shared<Node>(Node{id, value});
        ids_.insert(id);
        nodes_[id] = node;
        parent->children.push_front(node);
        return node;
    }

    std::string get_id(const int& index) const {
        return *ids_.find_by_order(index);
    }

    void delete_node(const std::string& id) {
        nodes_.erase(id);
        ids_.erase(id);
    }

    void build_tree(std::queue<std::shared_ptr<Node>>& nodes_queue, int& size_left) {
        if (size_left <= 0) {
            return;
        }

        std::shared_ptr<Node> node = nodes_queue.front();
        nodes_queue.pop();

        int node_size = std::min(size_left, MAX_NODE_SIZE);
        size_left -= node_size;
        for (int i = 0; i < node_size; ++i) {
            create_child(node);
        }
        for (const auto& child : node->children) {
            nodes_queue.push(child);
        }

        build_tree(nodes_queue,size_left);
    }

    void fill_tree(const std::vector<int>& values, int& index, const std::shared_ptr<Node>& node) {
        if (node != root_) {
            node->value = values[index++];
        }

        for (const auto& child : node->children) {
            fill_tree(values, index, child);
        }
    }
};