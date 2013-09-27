project(bunsan_binlogs)

include(${CMAKE_SOURCE_DIR}/system-config.cmake OPTIONAL)

cmake_minimum_required(VERSION 2.6)

include(BunsanCommon)

bunsan_use_boost(system filesystem thread)
bunsan_use_protobuf()

bunsan_protobuf_generate_cpp(SOURCES proto_srcs HEADERS proto_hdrs INSTALL include PROTOS
    bunsan/binlogs/v1/HeaderData.proto
)

bunsan_add_shared_library(${PROJECT_NAME}
    ${proto_srcs}

    src/common/error.cpp
    src/common/categorized_error.cpp
    src/common/system_error.cpp

    src/lib/Error.cpp
    src/lib/BaseIo.cpp
    src/lib/MessageType.cpp
    src/lib/MessageTypePool.cpp
    src/lib/LogReader.cpp
    src/lib/LogWriter.cpp
    src/lib/AbstractNamedLogWriter.cpp
    src/lib/LogFactory.cpp

    src/lib/detail/equals.cpp
    src/lib/detail/files.cpp
    src/lib/detail/format.cpp

    src/lib/directory_log/detail.cpp
    src/lib/directory_log/LogReader.cpp
    src/lib/directory_log/LogWriter.cpp

    src/lib/v1/format.cpp
    src/lib/v1/MessageType.cpp
    src/lib/v1/MessageTypePool.cpp
    src/lib/v1/LogReader.cpp
    src/lib/v1/BaseLogWriter.cpp
    src/lib/v1/LogWriter.cpp
    src/lib/v1/NamedLogWriter.cpp

    src/lib/io/BaseBuffer.cpp
    src/lib/io/ReadBuffer.cpp
    src/lib/io/WriteBuffer.cpp
    src/lib/io/file/AppendBuffer.cpp
    src/lib/io/file/ReadBuffer.cpp
    src/lib/io/file/WriteBuffer.cpp
    src/lib/io/file/open.cpp

    src/lib/io/filter/gzip.cpp
)
bunsan_use(${PROJECT_NAME})

bunsan_add_executable(${PROJECT_NAME}_read
    src/bin/read.cpp
)

bunsan_add_executable(${PROJECT_NAME}_fast_check
    src/bin/fast_check.cpp
)

bunsan_install_headers()
bunsan_install_targets(
    ${PROJECT_NAME}
    ${PROJECT_NAME}_read
    ${PROJECT_NAME}_fast_check
)

if(ENABLE_TESTS)
    bunsan_protobuf_generate_cpp(SOURCES tests_proto_srcs HEADERS proto_hdrs INSTALL include PROTOS
        bunsan/binlogs/tests/Message1.proto
        bunsan/binlogs/tests/Message2.proto
    )
    bunsan_add_shared_library(${PROJECT_NAME}_tests
        ${tests_proto_srcs}

        src/lib/tests/FileDescriptorSet.cpp
    )
endif()

bunsan_include_tests()
