#pragma once
#include <random>
#include <chrono>
std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

int gen_abs_int(int mn, int mx) {
    return rng() % (mx - mn + 1) + mn;
}

bool gen_bool() {
    return gen_abs_int(0, 1);
}

int gen_int(int mn, int mx) {
    if (mn > 0) {
        return gen_abs_int(mn, mx);
    } else if (mx < 0) {
        return -gen_abs_int(mn, mx);
    } else {
        bool is_neg = gen_bool();
        if (is_neg) {
            return -gen_abs_int(mn, 0);
        } else {
            return gen_abs_int(0, mx);
        }
    }
}

std::vector<int> gen_vector(int n, int mn, int mx) {
    std::vector<int> res(n);
    for(int i = 0; i < n; ++i) {
        res[i] = gen_int(mn, mx);
    }
    return res;
}

template <typename T>
T& pick_random_elem(std::vector<T>& arr) {
    return arr[gen_int(0, arr.size() - 1)];
}
