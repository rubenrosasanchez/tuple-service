cmake_minimum_required(VERSION 3.22)
project(Sockets C)

set(CMAKE_C_STANDARD 23)


add_executable(server server/server.c server/tuple_manager.c server/server_messaging.c commons.c)
add_executable(client client.c)

add_library(keys SHARED keys/keys.c commons.c)

target_link_libraries(server rt pthread)
target_link_libraries(client keys)
target_link_libraries(keys rt)
