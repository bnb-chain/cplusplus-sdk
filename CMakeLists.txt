cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(BinanceChain)

set (CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED)
find_package(Protobuf REQUIRED)

include(ExternalProject)
ExternalProject_Add(
    nlohmann_json
    PREFIX "nlohmann"
    URL https://github.com/nlohmann/json/releases/download/v3.5.0/include.zip
    URL_HASH SHA256=3564da9c5b0cf2e032f97c69baedf10ddbc98030c337d0327a215ea72259ea21
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ExternalProject_Get_property(nlohmann_json SOURCE_DIR)
include_directories(SOURCE_DIR)

ExternalProject_Add(
    pcg
    PREFIX "pcg"
    URL http://www.pcg-random.org/downloads/pcg-cpp-0.98.zip
    URL_HASH SHA256=de46a88aad4dfe54ef53f66beba56fd0ab3e94aaaa9860cd828b3f1daee13aaf
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
ExternalProject_Get_property(pcg SOURCE_DIR)
include_directories(${SOURCE_DIR}/include)

# Protobuf
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS src/dex.proto)

# Source files
file(GLOB_RECURSE sources src/crypto/*.c src/crypto/*.h src/*.cpp src/*.h)
add_library(BinanceChain ${sources} ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries(BinanceChain PRIVATE protobuf Boost::boost)

# Define headers for this library. PUBLIC headers are used for compiling the
# library, and will be added to consumers' build paths.
target_include_directories(BinanceChain
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_subdirectory(tests)
