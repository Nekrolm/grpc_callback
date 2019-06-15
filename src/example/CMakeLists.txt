cmake_minimum_required(VERSION 3.0)

project(GRPC_EXAMPLE)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)


find_package(Protobuf REQUIRED)
find_package(GRPC REQUIRED)


set(PROTOS  protos-src/test.proto)
set(GEN_PROTOS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/protos)

file(MAKE_DIRECTORY ${GEN_PROTOS_DIR})

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${GEN_PROTOS_DIR} ${PROTOS})
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${GEN_PROTOS_DIR} ${PROTOS})

execute_process(COMMAND  which grpc_cpp_plugin OUTPUT_VARIABLE GRPC_CPP_PLUGIN)

message(STATUS ${PROTO_SRCS})

add_executable(${PROJECT_NAME}  main.cpp
                                greating_client.cpp
                                greating_server.cpp
                                ${PROTO_SRCS}
                                ${GRPC_SRCS})


target_link_libraries(${PROJECT_NAME}
                      gRPC::grpc++_reflection
                      protobuf::libprotobuf)

