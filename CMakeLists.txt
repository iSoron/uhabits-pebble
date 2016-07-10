cmake_minimum_required(VERSION 3.5)
project(hello)

SET(PEBBLE_FLAVOUR basalt)
SET(PEBBLE_SDK_VER 3.14)
SET(PEBBLE_SDK "/opt/pebble-sdk")
INCLUDE_DIRECTORIES("${PEBBLE_SDK}/.pebble-sdk/SDKs/${PEBBLE_SDK_VER}/sdk-core/pebble/${PEBBLE_FLAVOUR}/include")
INCLUDE_DIRECTORIES("build/${PEBBLE_FLAVOUR}/src")
INCLUDE_DIRECTORIES("build/include")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        src/main.c
        src/network.c
        src/network.h
        src/util.h
        src/layers/border_layer.c
        src/layers/border_layer.h
        src/screens/list/list_window.c
        src/screens/list/list_window.h
        src/screens/list/style.h
        src/screens/action/action_window.c
        src/screens/action/action_window.h
        src/screens/action/style.h
        src/screens/action/action_menu_layer.c
        src/screens/action/action_menu_layer.h
        src/screens/list/list_layer.c
        src/screens/list/list_layer.h
        src/screens/animation/animation_window.c
        src/screens/animation/animation_window.h src/screens/animation/style.h)

add_executable(hello ${SOURCE_FILES})