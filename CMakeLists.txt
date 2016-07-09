cmake_minimum_required(VERSION 3.5)
project(hello)

SET(PEBBLE_FLAVOUR basalt)
SET(PEBBLE_SDK_VER 3.13.1)
SET(PEBBLE_SDK "/opt/pebble-sdk")
INCLUDE_DIRECTORIES("${PEBBLE_SDK}/.pebble-sdk/SDKs/${PEBBLE_SDK_VER}/sdk-core/pebble/${PEBBLE_FLAVOUR}/include")
INCLUDE_DIRECTORIES("build/${PEBBLE_FLAVOUR}/src")
INCLUDE_DIRECTORIES("build/include")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        src/main.c src/network.c src/network.h src/windows/list_window.c src/windows/list_window.h src/util.h)

add_executable(hello ${SOURCE_FILES})