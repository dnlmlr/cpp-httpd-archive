cmake_minimum_required(VERSION 3.10)

project(cpphttpd)

file(GLOB SRC_FILES "src/*.cpp")
file(GLOB HEADER_FILES "inc/*.hpp")

include_directories(inc)

add_library(cpphttpd STATIC ${SRC_FILES} ${HEADER_FILES})
set_target_properties(cpphttpd PROPERTIES PREFIX "")


add_executable(example.run EXCLUDE_FROM_ALL example/main.cpp)
target_link_libraries(example.run cpphttpd)
target_link_libraries(example.run pthread)
add_custom_target(example example.run)
