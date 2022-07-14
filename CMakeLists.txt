cmake_minimum_required(VERSION 3.16)
project(CRDT-Array)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(${PROJECT_NAME}
        main.cpp
        CrdtArray.h
        Generators.h
        Server.h
        Client.h)

target_link_libraries(${PROJECT_NAME} gtest_main)


