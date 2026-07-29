#!/bin/bash -eu

set -e

# Compiler settings
CC=clang
CFLAGS="-O1 -g -fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined"
LDFLAGS="-fsanitize=address -fsanitize=undefined"

# Source directory
SRC_DIR="src"
INCLUDE_DIR="include"
FUZZ_DIR="fuzz"
BUILD_DIR="build"
OUT_DIR="$OUT"

# Create build directory
mkdir -p "$BUILD_DIR"
mkdir -p "$OUT_DIR"

# Compile source files
echo "Compiling source files..."

# JIT compiler
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/jit/compiler/lexer.c" -o "$BUILD_DIR/lexer.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/jit/compiler/parser.c" -o "$BUILD_DIR/parser.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/jit/compiler/ast.c" -o "$BUILD_DIR/ast.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/jit/compiler/ir_generator.c" -o "$BUILD_DIR/ir_generator.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/jit/compiler/codegen.c" -o "$BUILD_DIR/codegen.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/jit/compiler/optimizer.c" -o "$BUILD_DIR/optimizer.o"

# JIT runtime
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/jit/runtime/executor.c" -o "$BUILD_DIR/executor.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/jit/runtime/memory_pool.c" -o "$BUILD_DIR/memory_pool.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/jit/runtime/gc.c" -o "$BUILD_DIR/gc.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/jit/runtime/native_bridge.c" -o "$BUILD_DIR/native_bridge.o"

# JIT reoptimizer
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/jit/reoptimizer/profiler.c" -o "$BUILD_DIR/profiler.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/jit/reoptimizer/hot_path.c" -o "$BUILD_DIR/hot_path.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/jit/reoptimizer/recompile.c" -o "$BUILD_DIR/recompile.o"

# Stream processing
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/stream/core/pipeline.c" -o "$BUILD_DIR/pipeline.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/stream/core/operator.c" -o "$BUILD_DIR/operator.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/stream/core/window.c" -o "$BUILD_DIR/window.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/stream/core/state.c" -o "$BUILD_DIR/state.o"

# Stream sources
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/stream/sources/socket_source.c" -o "$BUILD_DIR/socket_source.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/stream/sources/file_source.c" -o "$BUILD_DIR/file_source.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/stream/sources/memory_source.c" -o "$BUILD_DIR/memory_source.o"

# Stream sinks
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/stream/sinks/socket_sink.c" -o "$BUILD_DIR/socket_sink.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/stream/sinks/file_sink.c" -o "$BUILD_DIR/file_sink.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/stream/sinks/callback_sink.c" -o "$BUILD_DIR/callback_sink.o"

# Distributed coordination
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/distributed/coordination/node_discovery.c" -o "$BUILD_DIR/node_discovery.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/distributed/coordination/leader_election.c" -o "$BUILD_DIR/leader_election.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/distributed/coordination/partitioning.c" -o "$BUILD_DIR/partitioning.o"

# Distributed replication
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/distributed/replication/stream_replication.c" -o "$BUILD_DIR/stream_replication.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/distributed/replication/state_sync.c" -o "$BUILD_DIR/state_sync.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/distributed/replication/recovery.c" -o "$BUILD_DIR/recovery.o"

# Distributed consensus
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/distributed/consensus/raft.c" -o "$BUILD_DIR/raft.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/distributed/consensus/log.c" -o "$BUILD_DIR/log.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/distributed/consensus/quorum.c" -o "$BUILD_DIR/quorum.o"

# DSL parser
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/dsl/parser/query_parser.c" -o "$BUILD_DIR/query_parser.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/dsl/parser/expression.c" -o "$BUILD_DIR/expression.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/dsl/parser/validation.c" -o "$BUILD_DIR/validation.o"

# DSL stdlib
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/dsl/stdlib/operators.c" -o "$BUILD_DIR/operators.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/dsl/stdlib/functions.c" -o "$BUILD_DIR/functions.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/dsl/stdlib/aggregates.c" -o "$BUILD_DIR/aggregates.o"

# Utils memory
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/utils/memory/arena.c" -o "$BUILD_DIR/arena.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/utils/memory/pool.c" -o "$BUILD_DIR/pool.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/utils/memory/cache.c" -o "$BUILD_DIR/cache.o"

# Utils serialization
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/utils/serialization/codec.c" -o "$BUILD_DIR/codec.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/utils/serialization/frame.c" -o "$BUILD_DIR/frame.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/utils/serialization/protocol.c" -o "$BUILD_DIR/protocol.o"

# Utils crypto
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/utils/crypto/hash.c" -o "$BUILD_DIR/hash.o"
$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$SRC_DIR/utils/crypto/checksum.c" -o "$BUILD_DIR/checksum.o"

# Link object files
echo "Linking object files..."
$CC $LDFLAGS -o "$BUILD_DIR/libaether.a" \
    "$BUILD_DIR/lexer.o" \
    "$BUILD_DIR/parser.o" \
    "$BUILD_DIR/ast.o" \
    "$BUILD_DIR/ir_generator.o" \
    "$BUILD_DIR/codegen.o" \
    "$BUILD_DIR/optimizer.o" \
    "$BUILD_DIR/executor.o" \
    "$BUILD_DIR/memory_pool.o" \
    "$BUILD_DIR/gc.o" \
    "$BUILD_DIR/native_bridge.o" \
    "$BUILD_DIR/profiler.o" \
    "$BUILD_DIR/hot_path.o" \
    "$BUILD_DIR/recompile.o" \
    "$BUILD_DIR/pipeline.o" \
    "$BUILD_DIR/operator.o" \
    "$BUILD_DIR/window.o" \
    "$BUILD_DIR/state.o" \
    "$BUILD_DIR/socket_source.o" \
    "$BUILD_DIR/file_source.o" \
    "$BUILD_DIR/memory_source.o" \
    "$BUILD_DIR/socket_sink.o" \
    "$BUILD_DIR/file_sink.o" \
    "$BUILD_DIR/callback_sink.o" \
    "$BUILD_DIR/node_discovery.o" \
    "$BUILD_DIR/leader_election.o" \
    "$BUILD_DIR/partitioning.o" \
    "$BUILD_DIR/stream_replication.o" \
    "$BUILD_DIR/state_sync.o" \
    "$BUILD_DIR/recovery.o" \
    "$BUILD_DIR/raft.o" \
    "$BUILD_DIR/log.o" \
    "$BUILD_DIR/quorum.o" \
    "$BUILD_DIR/query_parser.o" \
    "$BUILD_DIR/expression.o" \
    "$BUILD_DIR/validation.o" \
    "$BUILD_DIR/operators.o" \
    "$BUILD_DIR/functions.o" \
    "$BUILD_DIR/aggregates.o" \
    "$BUILD_DIR/arena.o" \
    "$BUILD_DIR/pool.o" \
    "$BUILD_DIR/cache.o" \
    "$BUILD_DIR/codec.o" \
    "$BUILD_DIR/frame.o" \
    "$BUILD_DIR/protocol.o" \
    "$BUILD_DIR/hash.o" \
    "$BUILD_DIR/checksum.o"

# Compile fuzzers
echo "Compiling fuzzers..."

$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$FUZZ_DIR/dsl_parser_fuzzer.c" -o "$BUILD_DIR/dsl_parser_fuzzer.o"
$CC $CFLAGS $LDFLAGS -fsanitize=fuzzer "$BUILD_DIR/dsl_parser_fuzzer.o" "$BUILD_DIR/libaether.a" -o "$OUT_DIR/dsl_parser_fuzzer"

$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$FUZZ_DIR/jit_compiler_fuzzer.c" -o "$BUILD_DIR/jit_compiler_fuzzer.o"
$CC $CFLAGS $LDFLAGS -fsanitize=fuzzer "$BUILD_DIR/jit_compiler_fuzzer.o" "$BUILD_DIR/libaether.a" -o "$OUT_DIR/jit_compiler_fuzzer"

$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$FUZZ_DIR/stream_pipeline_fuzzer.c" -o "$BUILD_DIR/stream_pipeline_fuzzer.o"
$CC $CFLAGS $LDFLAGS -fsanitize=fuzzer "$BUILD_DIR/stream_pipeline_fuzzer.o" "$BUILD_DIR/libaether.a" -o "$OUT_DIR/stream_pipeline_fuzzer"

$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$FUZZ_DIR/distributed_protocol_fuzzer.c" -o "$BUILD_DIR/distributed_protocol_fuzzer.o"
$CC $CFLAGS $LDFLAGS -fsanitize=fuzzer "$BUILD_DIR/distributed_protocol_fuzzer.o" "$BUILD_DIR/libaether.a" -o "$OUT_DIR/distributed_protocol_fuzzer"

$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$FUZZ_DIR/replication_fuzzer.c" -o "$BUILD_DIR/replication_fuzzer.o"
$CC $CFLAGS $LDFLAGS -fsanitize=fuzzer "$BUILD_DIR/replication_fuzzer.o" "$BUILD_DIR/libaether.a" -o "$OUT_DIR/replication_fuzzer"

$CC $CFLAGS -I"$INCLUDE_DIR" -I"$SRC_DIR" -c "$FUZZ_DIR/memory_pool_fuzzer.c" -o "$BUILD_DIR/memory_pool_fuzzer.o"
$CC $CFLAGS $LDFLAGS -fsanitize=fuzzer "$BUILD_DIR/memory_pool_fuzzer.o" "$BUILD_DIR/libaether.a" -o "$OUT_DIR/memory_pool_fuzzer"

echo "Build complete."
