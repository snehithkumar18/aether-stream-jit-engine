cmake_minimum_required(VERSION 3.10)
project(AetherStreamJIT C)

set(CMAKE_C_STANDARD 11)
if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W4")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
endif()

# Fuzzer flags
set(FUZZER_FLAGS "-fsanitize=fuzzer,address,undefined")
set(FUZZER_LINK_FLAGS "-fsanitize=fuzzer,address,undefined")

# Core source files with bugs (reduced for faster fuzzing)
set(CORE_SOURCES
    src/jit/compiler/lexer.c
    src/jit/compiler/parser.c
    src/jit/compiler/codegen.c
    src/jit/compiler/optimizer.c
    src/jit/compiler/ir_generator.c
    src/jit/runtime/executor.c
    src/jit/runtime/memory_pool.c
    src/jit/runtime/gc.c
    src/stream/core/pipeline.c
    src/stream/core/window.c
    src/stream/core/state.c
    src/distributed/coordination/node_discovery.c
    src/distributed/coordination/leader_election.c
    src/distributed/coordination/partitioning.c
    src/distributed/replication/stream_replication.c
    src/distributed/replication/state_sync.c
    src/distributed/replication/recovery.c
    src/distributed/consensus/raft.c
    src/distributed/consensus/quorum.c
    src/dsl/parser/expression.c
    src/utils/memory/arena.c
    src/utils/memory/cache.c
    src/utils/serialization/codec.c
    src/utils/serialization/frame.c
    src/utils/serialization/protocol.c
)

# Include directories
include_directories(include)
include_directories(src/jit/compiler)
include_directories(src/jit/runtime)
include_directories(src/stream/core)
include_directories(src/distributed/coordination)
include_directories(src/distributed/replication)
include_directories(src/distributed/consensus)
include_directories(src/dsl/parser)
include_directories(src/utils/memory)
include_directories(src/utils/serialization)

# Main library
add_library(aether_stream STATIC ${CORE_SOURCES})

# Fuzzer targets (reduced to 3 for faster evaluation)
add_executable(jit_compiler_fuzzer fuzz/jit_compiler_fuzzer.c)
target_link_libraries(jit_compiler_fuzzer PRIVATE aether_stream)
target_compile_options(jit_compiler_fuzzer PRIVATE ${FUZZER_FLAGS})
target_link_options(jit_compiler_fuzzer PRIVATE ${FUZZER_LINK_FLAGS})

add_executable(stream_pipeline_fuzzer fuzz/stream_pipeline_fuzzer.c)
target_link_libraries(stream_pipeline_fuzzer PRIVATE aether_stream)
target_compile_options(stream_pipeline_fuzzer PRIVATE ${FUZZER_FLAGS})
target_link_options(stream_pipeline_fuzzer PRIVATE ${FUZZER_LINK_FLAGS})

add_executable(distributed_protocol_fuzzer fuzz/distributed_protocol_fuzzer.c)
target_link_libraries(distributed_protocol_fuzzer PRIVATE aether_stream)
target_compile_options(distributed_protocol_fuzzer PRIVATE ${FUZZER_FLAGS})
target_link_options(distributed_protocol_fuzzer PRIVATE ${FUZZER_LINK_FLAGS})
