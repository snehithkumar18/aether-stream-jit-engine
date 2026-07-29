#!/bin/bash
# Build script for fuzzing harnesses with libFuzzer

set -e

echo "Building Aether Stream JIT Engine fuzzers..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Debug \
         -DCMAKE_C_FLAGS="-fsanitize=fuzzer,address,undefined -g -O1" \
         -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=fuzzer,address,undefined"

# Build all fuzzers
cmake --build . --parallel

echo "Build complete. Fuzzers are now available in build/"
echo ""
echo "To run a fuzzer:"
echo "  ./build/dsl_parser_fuzzer ./fuzz/corpus/dsl_parser/"
echo "  ./build/jit_compiler_fuzzer ./fuzz/corpus/jit_compiler/"
echo "  ./build/stream_pipeline_fuzzer ./fuzz/corpus/stream_pipeline/"
echo "  ./build/distributed_protocol_fuzzer ./fuzz/corpus/distributed_protocol/"
echo "  ./build/replication_fuzzer ./fuzz/corpus/replication/"
echo "  ./build/memory_pool_fuzzer ./fuzz/corpus/memory_pool/"
