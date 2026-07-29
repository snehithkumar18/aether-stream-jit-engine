# Makefile for Aether Stream JIT Engine fuzzers
# Requires: gcc or clang with libFuzzer support

CC = gcc
CFLAGS = -fsanitize=fuzzer,address,undefined -g -O1 -Wall -Wextra
LDFLAGS = -fsanitize=fuzzer,address,undefined

# Include directories
INCLUDES = -Iinclude \
           -Isrc/jit/compiler \
           -Isrc/jit/runtime \
           -Isrc/jit/reoptimizer \
           -Isrc/stream/core \
           -Isrc/stream/sources \
           -Isrc/stream/sinks \
           -Isrc/distributed/coordination \
           -Isrc/distributed/replication \
           -Isrc/distributed/consensus \
           -Isrc/dsl/parser \
           -Isrc/dsl/stdlib \
           -Isrc/utils/memory \
           -Isrc/utils/serialization \
           -Isrc/utils/crypto

# Source files
SOURCES = src/jit/compiler/lexer.c \
          src/jit/compiler/parser.c \
          src/jit/compiler/ast.c \
          src/jit/compiler/codegen.c \
          src/jit/compiler/ir_generator.c \
          src/jit/compiler/optimizer.c \
          src/jit/runtime/executor.c \
          src/jit/runtime/memory_pool.c \
          src/jit/runtime/gc.c \
          src/jit/runtime/native_bridge.c \
          src/stream/core/pipeline.c \
          src/stream/core/operator.c \
          src/stream/core/window.c \
          src/stream/core/state.c \
          src/stream/sources/socket_source.c \
          src/stream/sources/file_source.c \
          src/stream/sources/memory_source.c \
          src/stream/sinks/file_sink.c \
          src/stream/sinks/callback_sink.c \
          src/distributed/coordination/node_discovery.c \
          src/distributed/coordination/leader_election.c \
          src/distributed/coordination/partitioning.c \
          src/distributed/replication/stream_replication.c \
          src/distributed/replication/state_sync.c \
          src/distributed/replication/recovery.c \
          src/distributed/consensus/raft.c \
          src/distributed/consensus/log.c \
          src/distributed/consensus/quorum.c \
          src/dsl/parser/query_parser.c \
          src/dsl/parser/expression.c \
          src/dsl/parser/validation.c \
          src/dsl/stdlib/operators.c \
          src/dsl/stdlib/functions.c \
          src/dsl/stdlib/aggregates.c \
          src/utils/memory/arena.c \
          src/utils/memory/pool.c \
          src/utils/memory/cache.c \
          src/utils/serialization/codec.c \
          src/utils/serialization/frame.c \
          src/utils/serialization/protocol.c \
          src/utils/crypto/hash.c \
          src/utils/crypto/checksum.c

# Fuzzer targets
FUZZERS = dsl_parser_fuzzer \
          jit_compiler_fuzzer \
          stream_pipeline_fuzzer \
          distributed_protocol_fuzzer \
          replication_fuzzer \
          memory_pool_fuzzer

.PHONY: all clean

all: $(FUZZERS)

dsl_parser_fuzzer: fuzz/dsl_parser_fuzzer.c $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

jit_compiler_fuzzer: fuzz/jit_compiler_fuzzer.c $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

stream_pipeline_fuzzer: fuzz/stream_pipeline_fuzzer.c $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

distributed_protocol_fuzzer: fuzz/distributed_protocol_fuzzer.c $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

replication_fuzzer: fuzz/replication_fuzzer.c $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

memory_pool_fuzzer: fuzz/memory_pool_fuzzer.c $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(FUZZERS)
