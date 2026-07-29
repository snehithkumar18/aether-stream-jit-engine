cmake_minimum_required(VERSION 3.10)
project(AetherStreamJIT C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

# Fuzzer flags
set(FUZZER_FLAGS "-fsanitize=fuzzer,address,undefined")
set(FUZZER_LINK_FLAGS "-fsanitize=fuzzer,address,undefined")

# Source files
set(JIT_COMPILER_SOURCES
    src/jit/compiler/lexer.c
    src/jit/compiler/parser.c
    src/jit/compiler/ast.c
    src/jit/compiler/codegen.c
    src/jit/compiler/ir_generator.c
    src/jit/compiler/optimizer.c
)

set(JIT_RUNTIME_SOURCES
    src/jit/runtime/executor.c
    src/jit/runtime/memory_pool.c
    src/jit/runtime/gc.c
    src/jit/runtime/native_bridge.c
)

set(STREAM_CORE_SOURCES
    src/stream/core/pipeline.c
    src/stream/core/operator.c
    src/stream/core/window.c
    src/stream/core/state.c
)

set(STREAM_SOURCES_SOURCES
    src/stream/sources/socket_source.c
    src/stream/sources/file_source.c
    src/stream/sources/memory_source.c
)

set(STREAM_SINKS_SOURCES
    src/stream/sinks/file_sink.c
    src/stream/sinks/callback_sink.c
)

set(DISTRIBUTED_COORDINATION_SOURCES
    src/distributed/coordination/node_discovery.c
    src/distributed/coordination/leader_election.c
    src/distributed/coordination/partitioning.c
)

set(DISTRIBUTED_REPLICATION_SOURCES
    src/distributed/replication/stream_replication.c
    src/distributed/replication/state_sync.c
    src/distributed/replication/recovery.c
)

set(DISTRIBUTED_CONSENSUS_SOURCES
    src/distributed/consensus/raft.c
    src/distributed/consensus/log.c
    src/distributed/consensus/quorum.c
)

set(DSL_PARSER_SOURCES
    src/dsl/parser/query_parser.c
    src/dsl/parser/expression.c
    src/dsl/parser/validation.c
)

set(DSL_STDLIB_SOURCES
    src/dsl/stdlib/operators.c
    src/dsl/stdlib/functions.c
    src/dsl/stdlib/aggregates.c
)

set(UTILS_MEMORY_SOURCES
    src/utils/memory/arena.c
    src/utils/memory/pool.c
    src/utils/memory/cache.c
)

set(UTILS_SERIALIZATION_SOURCES
    src/utils/serialization/codec.c
    src/utils/serialization/frame.c
    src/utils/serialization/protocol.c
)

set(UTILS_CRYPTO_SOURCES
    src/utils/crypto/hash.c
    src/utils/crypto/checksum.c
)

# All source files
set(ALL_SOURCES
    ${JIT_COMPILER_SOURCES}
    ${JIT_RUNTIME_SOURCES}
    ${STREAM_CORE_SOURCES}
    ${STREAM_SOURCES_SOURCES}
    ${STREAM_SINKS_SOURCES}
    ${DISTRIBUTED_COORDINATION_SOURCES}
    ${DISTRIBUTED_REPLICATION_SOURCES}
    ${DISTRIBUTED_CONSENSUS_SOURCES}
    ${DSL_PARSER_SOURCES}
    ${DSL_STDLIB_SOURCES}
    ${UTILS_MEMORY_SOURCES}
    ${UTILS_SERIALIZATION_SOURCES}
    ${UTILS_CRYPTO_SOURCES}
)

# Include directories
include_directories(include)
include_directories(src/jit/compiler)
include_directories(src/jit/runtime)
include_directories(src/jit/reoptimizer)
include_directories(src/stream/core)
include_directories(src/stream/sources)
include_directories(src/stream/sinks)
include_directories(src/distributed/coordination)
include_directories(src/distributed/replication)
include_directories(src/distributed/consensus)
include_directories(src/dsl/parser)
include_directories(src/dsl/stdlib)
include_directories(src/utils/memory)
include_directories(src/utils/serialization)
include_directories(src/utils/crypto)

# Main library
add_library(aether_stream STATIC ${ALL_SOURCES})

# Fuzzer targets
add_executable(dsl_parser_fuzzer fuzz/dsl_parser_fuzzer.c ${ALL_SOURCES})
target_compile_options(dsl_parser_fuzzer PRIVATE ${FUZZER_FLAGS})
target_link_options(dsl_parser_fuzzer PRIVATE ${FUZZER_LINK_FLAGS})

add_executable(jit_compiler_fuzzer fuzz/jit_compiler_fuzzer.c ${ALL_SOURCES})
target_compile_options(jit_compiler_fuzzer PRIVATE ${FUZZER_FLAGS})
target_link_options(jit_compiler_fuzzer PRIVATE ${FUZZER_LINK_FLAGS})

add_executable(stream_pipeline_fuzzer fuzz/stream_pipeline_fuzzer.c ${ALL_SOURCES})
target_compile_options(stream_pipeline_fuzzer PRIVATE ${FUZZER_FLAGS})
target_link_options(stream_pipeline_fuzzer PRIVATE ${FUZZER_LINK_FLAGS})

add_executable(distributed_protocol_fuzzer fuzz/distributed_protocol_fuzzer.c ${ALL_SOURCES})
target_compile_options(distributed_protocol_fuzzer PRIVATE ${FUZZER_FLAGS})
target_link_options(distributed_protocol_fuzzer PRIVATE ${FUZZER_LINK_FLAGS})

add_executable(replication_fuzzer fuzz/replication_fuzzer.c ${ALL_SOURCES})
target_compile_options(replication_fuzzer PRIVATE ${FUZZER_FLAGS})
target_link_options(replication_fuzzer PRIVATE ${FUZZER_LINK_FLAGS})

add_executable(memory_pool_fuzzer fuzz/memory_pool_fuzzer.c ${ALL_SOURCES})
target_compile_options(memory_pool_fuzzer PRIVATE ${FUZZER_FLAGS})
target_link_options(memory_pool_fuzzer PRIVATE ${FUZZER_LINK_FLAGS})

# ClusterFuzzLite configuration
set(FUZZER_LDFLAGS "-fsanitize=fuzzer,address,undefined")
set(FUZZER_CFLAGS "-fsanitize=fuzzer,address,undefined -g -O1")
