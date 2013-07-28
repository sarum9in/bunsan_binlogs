project(bunsan_binlogs)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.8)

include(BunsanCommon)

bunsan_use_boost(system filesystem thread)
bunsan_use_protobuf()

bunsan_protobuf_generate_cpp(SOURCES proto_srcs HEADERS proto_hdrs INSTALL include PROTOS
    bunsan/binlogs/v1/HeaderData.proto
)

bunsan_add_shared_library(${PROJECT_NAME}
    ${proto_srcs}

    src/lib/BaseIo.cpp
    src/lib/BaseBuffer.cpp
    src/lib/MessageType.cpp
    src/lib/MessageTypePool.cpp
    src/lib/LogReader.cpp
    src/lib/LogWriter.cpp
    src/lib/AbstractNamedLogWriter.cpp
    src/lib/LogFactory.cpp

    src/lib/v1/MessageType.cpp
    src/lib/v1/MessageTypePool.cpp
    src/lib/v1/LogWriter.cpp
    src/lib/v1/LogReader.cpp

    src/lib/file/BaseBuffer.cpp
    src/lib/file/ReadBuffer.cpp
    src/lib/file/WriteBuffer.cpp
    src/lib/file/open.cpp
)
bunsan_use(${PROJECT_NAME})

bunsan_add_executable(${PROJECT_NAME}_read
    src/bin/read.cpp
)

bunsan_install_headers()
bunsan_install_targets(${PROJECT_NAME} ${PROJECT_NAME}_read)

bunsan_include_tests()
