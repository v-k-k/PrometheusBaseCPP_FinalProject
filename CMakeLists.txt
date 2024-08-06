cmake_minimum_required(VERSION 3.10)
project(communicate)

set(CMAKE_CXX_STANDARD 17)
set(SOURCE_FILES
    src/exceptions/customEx.cpp
    src/tools/tools.cpp
    src/client/client.cpp
    src/server/server.cpp
    src/main.cpp
)

add_executable(communicate ${SOURCE_FILES})
