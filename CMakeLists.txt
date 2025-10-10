cmake_minimum_required(VERSION 3.28)
project(Steganography_project)

set(CMAKE_CXX_STANDARD 20)


#https://stackoverflow.com/questions/52892556/clion-build-program-wont-run-in-cmd
#if this isn't in code, it could run, but not from command line, I would have had to use commands via three dots on top right of CLion
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-static")

include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG 10.2.1
)

FetchContent_MakeAvailable(fmt)

add_executable(Steganography_project main.cpp
        ImageHandler.cpp
        Steganography.cpp
        ImageHandler.cpp
        ImageHandler.hpp
        ImageHandler.hpp
        ImageHandler.cpp
        Steganography.cpp
        Steganography.hpp)

target_link_libraries(Steganography_project fmt)
