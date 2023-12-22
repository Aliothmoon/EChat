cmake_minimum_required(VERSION 3.27)
project(EChat)

set(CMAKE_CXX_STANDARD 14)

add_executable(server server.cpp map/SyncMap.h map/SyncMap.cpp
        executor/pool.cpp executor/pool.h
        queue/SyncQueue.cpp queue/SyncQueue.h
)
add_executable(client client.cpp)