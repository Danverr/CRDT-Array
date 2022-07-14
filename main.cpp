#include <iostream>
#include <optional>
#include <gtest/gtest.h>
#include "CrdtArray.h"
#include "Generators.h"

#define SMALL_ARRAY_SIZE 2000
#define BIG_ARRAY_SIZE 100000
#define OPERATIONS_COUNT 1000
#define MAX_VAL 1e9
#define MIN_VAL -1e9

void compare_arrays(CrdtArray& crdt_array, std::vector<int>& vec) {
    ASSERT_EQ(vec.size(), crdt_array.size());
    for (int i = 0; i < crdt_array.size(); ++i) {
        ASSERT_EQ(vec[i], crdt_array[i]);
    }
}

void compare_arrays(CrdtArray& crdt_array1, CrdtArray& crdt_array2) {
    ASSERT_EQ(crdt_array1.size(), crdt_array2.size());
    for (int i = 0; i < crdt_array1.size(); ++i) {
        ASSERT_EQ(crdt_array1[i], crdt_array2[i]);
    }
}

std::pair<int, int> insert(CrdtArray& crdt_array, std::vector<int>& vec, std::optional<int> opt_index, std::optional<int> opt_value) {
    int index = opt_index.value_or(gen_abs_int(0, vec.size() - 1));
    int value = opt_value.value_or(gen_int(MIN_VAL, MAX_VAL));
    vec.insert(vec.begin() + index, value);
    crdt_array.insert(index - 1, value);
    return {index, value};
}

int erase(CrdtArray& crdt_array, std::vector<int>& vec, std::optional<int> opt_index) {
    int index = opt_index.value_or(gen_abs_int(0, vec.size() - 1));
    vec.erase(vec.begin() + index);
    crdt_array.erase(index);
    return index;
}

std::pair<int, int> update(CrdtArray& crdt_array, std::vector<int>& vec, std::optional<int> opt_index, std::optional<int> opt_value) {
    int index = opt_index.value_or(gen_abs_int(0, vec.size() - 1));
    int value = opt_value.value_or(gen_int(MIN_VAL, MAX_VAL));
    vec[index] = value;
    crdt_array.update(index, value, 0);
    return {index, value};
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
        insert(crdt_array, vec, std::nullopt, std::nullopt);
    }

    compare_arrays(crdt_array, vec);
}

TEST(CrdtArrayTests, Erase) {
    std::vector<int> vec = gen_vector(SMALL_ARRAY_SIZE, MIN_VAL, MAX_VAL);
    CrdtArray crdt_array(vec);

    for (int i = 0; i < OPERATIONS_COUNT; ++i) {
        erase(crdt_array, vec, std::nullopt);
    }

    compare_arrays(crdt_array, vec);
}

TEST(CrdtArrayTests, Update) {
    std::vector<int> vec = gen_vector(SMALL_ARRAY_SIZE, MIN_VAL, MAX_VAL);
    CrdtArray crdt_array(vec);

    for (int i = 0; i < OPERATIONS_COUNT; ++i) {
        update(crdt_array, vec, std::nullopt, std::nullopt);
    }

    compare_arrays(crdt_array, vec);
}

TEST(CrdtArrayTests, FirstElementOperations) {
    std::vector<int> vec = gen_vector(SMALL_ARRAY_SIZE, MIN_VAL, MAX_VAL);
    CrdtArray crdt_array(vec);

    insert(crdt_array, vec, 0, std::nullopt);
    ASSERT_EQ(vec.size(), crdt_array.size());
    ASSERT_EQ(vec[0], crdt_array[0]);

    update(crdt_array, vec, 0, std::nullopt);
    ASSERT_EQ(vec.size(), crdt_array.size());
    ASSERT_EQ(vec[0], crdt_array[0]);

    erase(crdt_array, vec, 0);
    ASSERT_EQ(vec.size(), crdt_array.size());
    ASSERT_EQ(vec[0], crdt_array[0]);
}

TEST(CrdtArrayTests, LastElementOperations) {
    std::vector<int> vec = gen_vector(SMALL_ARRAY_SIZE, MIN_VAL, MAX_VAL);
    CrdtArray crdt_array(vec);

    insert(crdt_array, vec, vec.size() - 1, std::nullopt);
    ASSERT_EQ(vec.size(), crdt_array.size());
    ASSERT_EQ(vec[0], crdt_array[0]);

    update(crdt_array, vec, vec.size() - 1, std::nullopt);
    ASSERT_EQ(vec.size(), crdt_array.size());
    ASSERT_EQ(vec[0], crdt_array[0]);

    erase(crdt_array, vec, vec.size() - 1);
    ASSERT_EQ(vec.size(), crdt_array.size());
    ASSERT_EQ(vec[0], crdt_array[0]);
}

TEST(CrdtArrayTests, AllOperations) {
    std::vector<int> vec = gen_vector(SMALL_ARRAY_SIZE, MIN_VAL, MAX_VAL);
    CrdtArray crdt_array(vec);

    for (int i = 0; i < OPERATIONS_COUNT; ++i) {
        int operation = gen_abs_int(0, 2);

        if (operation == 0) {
            insert(crdt_array, vec, std::nullopt, std::nullopt);
        } else if (operation == 1) {
            if (vec.empty()) {
                --i;
            } else {
                erase(crdt_array, vec, std::nullopt);
            }
        } else if (operation == 2) {
            update(crdt_array, vec, std::nullopt, std::nullopt);
        }
    }

    compare_arrays(crdt_array, vec);
}

TEST(CrdtArrayPerformanceTests, CreateArray) {
    std::vector<int> vec = gen_vector(BIG_ARRAY_SIZE, MIN_VAL, MAX_VAL);
    CrdtArray crdt_array(vec);
    compare_arrays(crdt_array, vec);
}

TEST(CrdtArrayPerformanceTests, OfflineOperations) {
    std::vector<int> vec = gen_vector(BIG_ARRAY_SIZE, MIN_VAL, MAX_VAL);
    std::vector<CrdtArray> crdt_arrays = std::vector<CrdtArray>{CrdtArray(vec, "A"), CrdtArray(vec, "B")};

    struct Operation {
        int operation;
        std::string id;
        int value;
        int ts;
    };
    std::vector<Operation> operations[2];

    int ts_counter = 0;
    for (int i = 0; i < crdt_arrays.size(); ++i) {
        for (int j = 0; j < OPERATIONS_COUNT; ++j) {
            int operation = gen_abs_int(0, 2);
            int index = gen_abs_int(0, crdt_arrays[i].size() - 1);
            int value = gen_int(MIN_VAL, MAX_VAL);

            std::string id;
            if (operation == 0) {
                id = crdt_arrays[i].insert(index, value);
            } else if (operation == 1) {
                if (crdt_arrays[i].size() == 0) {
                    --i;
                    continue;
                }
                id = crdt_arrays[i].erase(index);
            } else if (operation == 2) {
                id = crdt_arrays[i].update(index, value, ts_counter);
            }

            ++ts_counter;
            operations[i].push_back(Operation{operation, id, value, ts_counter});
        }
    }

    for (int i = 0; i < crdt_arrays.size(); ++i) {
        for (int j = 0; j < operations[i ^ 1].size(); ++j) {
            auto [operation, id, value, ts] = operations[i ^ 1][j];

            if (operation == 0) {
                crdt_arrays[i].insert(id, value);
            } else if (operation == 1) {
                crdt_arrays[i].erase(id);
            } else if (operation == 2) {
                crdt_arrays[i].update(id, value, ts);
            }
        }
    }

    compare_arrays(crdt_arrays[0], crdt_arrays[1]);
}

GTEST_API_ int main(int argc, char ** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
