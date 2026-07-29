# Aether Stream JIT Engine

A high-performance stream processing engine with just-in-time compilation for real-time data analytics and distributed computing.

## Features

- **JIT Compiler**: Lexer, parser, AST, IR generation, code generation, and optimization
- **Stream Processing**: Pipeline operators, windowing, state management
- **Distributed Coordination**: Node discovery, leader election, partitioning
- **Distributed Consensus**: Raft protocol implementation with logs and quorum management
- **Replication**: Stream replication, state synchronization, and recovery mechanisms
- **DSL Parser**: Query parser, expression evaluation, validation
- **Memory Management**: Arena allocator, memory pools, and caching
- **Serialization**: Codec, frame, and protocol layers with checksums

## Building

### Prerequisites

- CMake 3.10 or higher
- C compiler with C11 support (GCC, Clang, or MSVC)
- For fuzzing: libFuzzer or AFL

### Standard Build

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Fuzzing Build

```bash
mkdir build
cd build
cmake -DFUZZ=ON ..
cmake --build .
```

## Fuzzing

The project includes comprehensive fuzzing harnesses for all major components:

- `dsl_parser_fuzzer` - DSL query parser and expression evaluator
- `jit_compiler_fuzzer` - JIT compiler components (lexer, parser, codegen, IR)
- `stream_pipeline_fuzzer` - Stream processing pipeline and operators
- `distributed_protocol_fuzzer` - Distributed coordination and consensus protocols
- `replication_fuzzer` - Replication, state sync, and recovery mechanisms
- `memory_pool_fuzzer` - Memory utilities (arena, pool, cache)

### Running Fuzzers

Each fuzzer has a seed corpus in `fuzz/corpus/<fuzzer_name>/`.

```bash
# Run with libFuzzer
./dsl_parser_fuzzer fuzz/corpus/dsl_parser/

# Run with AFL
afl-fuzz -i fuzz/corpus/dsl_parser/ -o findings/ ./dsl_parser_fuzzer @@
```

## Project Structure

```
src/
├── jit/
│   ├── compiler/    # Lexer, parser, AST, IR, codegen, optimizer
│   ├── runtime/     # Executor, memory pool, GC, native bridge
│   └── reoptimizer/ # Runtime reoptimization
├── stream/
│   ├── core/        # Pipeline, operators, windows, state
│   ├── sources/     # Socket, file, memory sources
│   └── sinks/       # File, callback sinks
├── distributed/
│   ├── coordination/ # Node discovery, leader election, partitioning
│   ├── replication/  # Stream replication, state sync, recovery
│   └── consensus/    # Raft protocol, logs, quorum
├── dsl/
│   ├── parser/      # Query parser, expressions, validation
│   └── stdlib/      # Operators, functions, aggregates
└── utils/
    ├── memory/      # Arena allocator, pool, cache
    ├── serialization/ # Codec, frame, protocol
    └── crypto/      # Hash, checksum
```

## License

MIT License
