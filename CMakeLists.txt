project(bunsan_binlogs)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(BunsanCommon)

bunsan_use_boost(system filesystem thread)
bunsan_use_protobuf()

bunsan_add_shared_library(${PROJECT_NAME}
    src/lib/stub.cpp
)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME})

bunsan_include_tests()
