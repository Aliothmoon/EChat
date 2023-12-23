cmake_minimum_required(VERSION 3.22.1)
project(EChat)

set(CMAKE_CXX_STANDARD 14)

add_executable(server server.cpp map/SyncMap.h
        executor/pool.cpp executor/pool.h
        queue/SyncQueue.cpp queue/SyncQueue.h
        poller/poller.cpp poller/poller.h
        handler/user.h list/list.h
)
add_executable(client client.cpp)