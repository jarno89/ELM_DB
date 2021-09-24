cmake_minimum_required(VERSION 3.16)
project(elm_project C)

set(CMAKE_C_STANDARD 99)
set(COMMON_SOURCES main.c cluster.c cluster.h util.c util.h data_types.h state.c state.h elm_decision_set.c elm_decision_set.h)

add_executable(elm ${COMMON_SOURCES})
add_executable(elm_db ${COMMON_SOURCES})
set_target_properties(elm PROPERTIES COMPILE_DEFINITIONS "DECISION_BRANCHING=0")
set_target_properties(elm_db PROPERTIES COMPILE_DEFINITIONS "DECISION_BRANCHING=1")

target_link_libraries(elm m)
target_link_libraries(elm_db m)
