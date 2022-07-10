#include <iostream>
#include <gtest/gtest.h>
#include "CrdtArray.h"
#include "Generators.h"

#define SMALL_ARRAY_SIZE 2000
#define OPERATIONS_COUNT 1000
#define MAX_VAL 1e9
#define MIN_VAL -1e9

void compare_arrays(CrdtArray& crdt_array, std::vector<int>& vec) {
    ASSERT_EQ(vec.size(), crdt_array.size());
    for (int i = 0; i < crdt_array.size(); ++i) {
        ASSERT_EQ(vec[i], crdt_array[i]);
    }
}

TEST(CrdtArrayTests, CreateArray) {
    std::vector<int> vec = gen_vector(SMALL_ARRAY_SIZE, MIN_VAL, MAX_VAL);
    CrdtArray crdt_array(vec);
    compare_arrays(crdt_array, vec);
}

TEST(CrdtArrayTests, Insert) {
    std::vector<int> vec = gen_vector(SMALL_ARRAY_SIZE, MIN_VAL, MAX_VAL);
    CrdtArray crdt_array(vec);

    for (int i = 0; i < OPERATIONS_COUNT; ++i) {
        int index = gen_abs_int(0, vec.size() - 1);
        int value = gen_int(MIN_VAL, MAX_VAL);
        vec.insert(vec.begin() + index, value);
        crdt_array.insert(index - 1, value);
    }

    compare_arrays(crdt_array, vec);
}

TEST(CrdtArrayTests, Erase) {
    std::vector<int> vec = gen_vector(SMALL_ARRAY_SIZE, MIN_VAL, MAX_VAL);
    CrdtArray crdt_array(vec);

    for (int i = 0; i < OPERATIONS_COUNT; ++i) {
        int index = gen_abs_int(0, vec.size() - 1);
        vec.erase(vec.begin() + index);
        crdt_array.erase(index);
    }

    compare_arrays(crdt_array, vec);
}

TEST(CrdtArrayTests, Update) {
    std::vector<int> vec = gen_vector(SMALL_ARRAY_SIZE, MIN_VAL, MAX_VAL);
    CrdtArray crdt_array(vec);

    for (int i = 0; i < OPERATIONS_COUNT; ++i) {
        int index = gen_abs_int(0, vec.size() - 1);
        int value = gen_int(MIN_VAL, MAX_VAL);
        vec[index] = value;
        crdt_array.update(index, value);
    }

    compare_arrays(crdt_array, vec);
}

TEST(CrdtArrayTests, AllOperations) {
    std::vector<int> vec = gen_vector(SMALL_ARRAY_SIZE, MIN_VAL, MAX_VAL);
    CrdtArray crdt_array(vec);

    for (int i = 0; i < OPERATIONS_COUNT; ++i) {
        int operation = gen_abs_int(0, 2);
        int index = gen_abs_int(0, vec.size() - 1);
        int value = gen_int(MIN_VAL, MAX_VAL);
        std::cout << i << ": code=" << operation << ", index=" << index << ", value=" << value << std::endl;

        if (operation == 0) {
            vec.insert(vec.begin() + index, value);
            crdt_array.insert(index - 1, value);
        } else if (operation == 1) {
            if (vec.empty()) {
                --i;
            } else {
                vec.erase(vec.begin() + index);
                crdt_array.erase(index);
            }
        } else if (operation == 2) {
            vec[index] = value;
            crdt_array.update(index, value);
        }

        std::cout << "vec_size=" << vec.size() << ", crdt_size=" << crdt_array.size() << std::endl;
        ASSERT_EQ(vec.size(), crdt_array.size());
        ASSERT_EQ(vec[index], crdt_array[index]);
    }
}

TEST(CrdtArrayTests, FrontElementOperations) {
    std::vector<int> vec = gen_vector(SMALL_ARRAY_SIZE, MIN_VAL, MAX_VAL);
    CrdtArray crdt_array(vec);

    int value = gen_int(MIN_VAL, MAX_VAL);
    vec.insert(vec.begin(), value);
    crdt_array.insert(-1, value);
    ASSERT_EQ(vec.size(), crdt_array.size());
    ASSERT_EQ( vec[0], crdt_array[0]);

    value = gen_int(MIN_VAL, MAX_VAL);
    vec[0] = value;
    crdt_array.update(0 , value);
    ASSERT_EQ(vec.size(), crdt_array.size());
    ASSERT_EQ( vec[0], crdt_array[0]);

    vec.erase(vec.begin());
    crdt_array.erase(0);
    ASSERT_EQ(vec.size(), crdt_array.size());
    ASSERT_EQ( vec[0], crdt_array[0]);
}

GTEST_API_ int main(int argc, char ** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}