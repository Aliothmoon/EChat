cmake_minimum_required(VERSION 3.16.3)
project(EChat)

set(CMAKE_CXX_STANDARD 11)


set(CMAKE_CXX_FLAGS -pthread)
message(STATUS "CMAKE_CXX_FLAGS = ${CMAKE_CXX_FLAGS}")

add_executable(server server.cpp map/SyncMap.h
        executor/pool.cpp executor/pool.h
        queue/SyncQueue.cpp queue/SyncQueue.h
        poller/poller.cpp poller/poller.h
        handler/user.h list/list.h
)
add_executable(client client.cpp)

