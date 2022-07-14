#pragma once
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <ext/rope>
#include <utility>
#include <vector>
#include <queue>
#include <string>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <random>
#include <chrono>
using namespace __gnu_pbds;
using namespace __gnu_cxx;

class CrdtArray {

 public:

    explicit CrdtArray(std::string replica_id = "") : REPLICA_ID(std::move(replica_id)) {
        random = std::mt19937(std::chrono::steady_clock::now().time_since_epoch().count());
    }

    explicit CrdtArray(const std::vector<int>& vec, const std::string& replica_id = "") : CrdtArray(replica_id) {
        std::vector<std::string> queue;

        for (int i = -1, size_left = vec.size(); queue.size() < vec.size(); ++i) {
            std::string id = (i == -1 ? "" : queue[i]);
            for (int j = MIN_CHAR + 1; j <= MAX_CHAR && queue.size() < vec.size(); ++j) {
                queue.push_back(id + char(j));
            }
        }

        sort(queue.begin(), queue.end());

        for (int i = 0; i < queue.size(); ++i) {
            insert(add_replica_id(queue[i], DEFAULT_ID), vec[i]);
        }
    }

    std::string insert(const int& index, const int& value) {
        return insert(index, value, REPLICA_ID);
    }

    std::string insert(const int& index, const int& value, const std::string& replica_id) {
        std::string parent_id = (size() == 0 || index == -1 ?
            std::string(1, char(MIN_CHAR)) : trim_replica_id(get_id(index)));
        std::string new_id = parent_id;

        while(true) {
            if (next_available_id_[new_id].empty()) {
                next_available_id_[new_id] = new_id + char(CrdtArray::MAX_CHAR);
            }

            if (next_available_id_[new_id] != new_id + char(MIN_CHAR)) {
                new_id = next_available_id_[new_id];
                break;
            } else {
                new_id += char(MIN_CHAR);
            }
        }

        return insert(add_replica_id(new_id, replica_id), value);
    }

    std::string insert(const std::string& id, const int& value) {
        std::string original_id = trim_replica_id(id);
        std::string parent_id = original_id;
        parent_id.pop_back();

        values_[id] = value;
        ids_.insert(id);

        std::string new_next_available_id_ = original_id;
        new_next_available_id_[original_id.size() - 1] = char(int(original_id.back()) - 1);
        next_available_id_[parent_id] = std::min(next_available_id_[parent_id], new_next_available_id_);

        if (!next_available_id_.contains(original_id)) {
            next_available_id_[original_id] = original_id + char(CrdtArray::MAX_CHAR);
        }

        return id;
    }

    std::string erase(const int& index) {
        return erase(get_id(index));
    }

    std::string erase(const std::string& id) {
        ids_.erase(id);
        return id;
    }

    std::string update(const int& index, const int& value, const int& ts) {
        return update(get_id(index), value, ts);
    }

    std::string update(const std::string& id, const int& value, const int& ts) {
        if (last_update_ts_[id] <= ts) {
            values_[id] = value;
            last_update_ts_[id] = ts;
        }
        return id;
    }

    int operator[](const int& index) {
        return values_[get_id(index)];
    }

    size_t size() const {
        return ids_.size();
    }

    std::string get_id(const int& index) const {
        return *ids_.find_by_order(index);
    }

 private:

    template <typename T>
    using ordered_set = tree<T, null_type, std::less<T>, rb_tree_tag, tree_order_statistics_node_update>;

    const std::string REPLICA_ID;
    inline static const std::string DEFAULT_ID;
    static const char ID_SEPARATOR = ' '; // char(32)
    static const int MIN_CHAR = 33;
    static const int MAX_CHAR = 126;
    std::mt19937 random;

    ordered_set<std::string> ids_;
    std::unordered_map<std::string, int> values_;
    std::unordered_map<std::string, int> last_update_ts_;
    std::unordered_map<std::string, std::string> next_available_id_;

    static std::string trim_replica_id(std::string id) {
        while (id.back() != ID_SEPARATOR) {
            id.pop_back();
        }
        id.pop_back();
        return id;
    }

    std::string add_replica_id(const std::string& id, const std::string& replica_id) const {
        return id + ID_SEPARATOR + replica_id;
    }

    void build_tree(std::queue<std::string>& queue, int& size_left) {
        if (size_left <= 0) {
            return;
        }

        std::string id = queue.front();
        queue.pop();

        int node_size = std::min(size_left, MAX_CHAR - MIN_CHAR);
        size_left -= node_size;

        for (int i = 0; i < node_size; ++i) {
            std::string new_id = id + char(MIN_CHAR + i + 1);
            ids_.insert(new_id);
            if (queue.size() < size_left) {
                queue.push(new_id);
            }
        }

        build_tree(queue, size_left);
    }

};