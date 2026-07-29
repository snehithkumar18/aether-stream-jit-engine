# Build Instructions

This document provides detailed instructions for building and fuzzing the Aether Stream JIT Engine.

## Prerequisites

### Required Tools

- **C Compiler**: GCC 7+, Clang 5+, or MSVC 2017+
- **CMake**: Version 3.10 or higher
- **Make**: (optional, for Makefile-based builds)
- **Git**: For cloning the repository

### For Fuzzing

- **libFuzzer**: Built into Clang, or available as a standalone library
- **AddressSanitizer**: Supported by GCC and Clang
- **UndefinedBehaviorSanitizer**: Supported by GCC and Clang

### Installing Prerequisites

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install build-essential cmake git
sudo apt-get install libubsan-dev libasan-dev
```

#### macOS
```bash
brew install cmake git
```

#### Windows
- Install Visual Studio 2017 or later with C++ support
- Install CMake from https://cmake.org/download/
- Install Git from https://git-scm.com/

## Building with CMake

### Standard Build (Debug)

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

### Standard Build (Release)

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### Fuzzing Build with libFuzzer

#### Linux/macOS with Clang
```bash
mkdir build
cd build
CC=clang CXX=clang++ cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_FLAGS="-fsanitize=fuzzer,address,undefined -g -O1" \
    -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=fuzzer,address,undefined"
cmake --build .
```

#### Linux with GCC
```bash
mkdir build
cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_FLAGS="-fsanitize=address,undefined -g -O1" \
    -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"
cmake --build .
```

#### Windows with MSVC
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug
```

Note: MSVC has limited libFuzzer support. Consider using Clang on Windows for better fuzzing support.

## Building with Makefile

The project includes a Makefile for simple builds with GCC or Clang.

### Build All Fuzzers
```bash
make
```

### Build Specific Fuzzer
```bash
make dsl_parser_fuzzer
make jit_compiler_fuzzer
make stream_pipeline_fuzzer
make distributed_protocol_fuzzer
make replication_fuzzer
make memory_pool_fuzzer
```

### Clean Build Artifacts
```bash
make clean
```

## Using Build Scripts

### Linux/macOS
```bash
chmod +x build_fuzzers.sh
./build_fuzzers.sh
```

### Windows
```cmd
build_fuzzers.bat
```

## Running Fuzzers

### With libFuzzer

```bash
# Basic run
./dsl_parser_fuzzer fuzz/corpus/dsl_parser/

# With timeout (30 minutes)
timeout 1800 ./dsl_parser_fuzzer fuzz/corpus/dsl_parser/

# With specific options
./dsl_parser_fuzzer -max_total_time=1800 -workers=4 fuzz/corpus/dsl_parser/
```

### With AFL

```bash
# First, compile with AFL instrumentation
afl-clang-fast -o dsl_parser_fuzzer fuzz/dsl_parser_fuzzer.c $(SOURCES) $(CFLAGS)

# Run AFL
afl-fuzz -i fuzz/corpus/dsl_parser/ -o findings/ ./dsl_parser_fuzzer @@
```

### With ClusterFuzzLite

The project includes a `.clusterfuzzlite` directory for ClusterFuzzLite integration.

```bash
# Install ClusterFuzzLite
pip install clusterfuzzlite

# Run ClusterFuzzLite
cifuzz --engine libfuzzer dsl_parser_fuzzer fuzz/corpus/dsl_parser/
```

## Fuzzer Corpora

Each fuzzer has a seed corpus in `fuzz/corpus/<fuzzer_name>/`:

- `dsl_parser/` - Valid queries, invalid tokens, complex ASTs, unicode, special characters
- `jit_compiler/` - Basic and complex data for JIT components
- `stream_pipeline/` - Stream processing data
- `distributed_protocol/` - Protocol messages and coordination data
- `replication/` - Replication and recovery data
- `memory_pool/` - Memory allocation patterns

## Troubleshooting

### CMake Not Found
Ensure CMake is installed and in your PATH:
```bash
cmake --version
```

### Compiler Not Found
Ensure your compiler is installed and accessible:
```bash
gcc --version  # or clang --version
```

### Sanitizer Errors
If you encounter sanitizer errors during build:
- Ensure your compiler supports the requested sanitizers
- Try using Clang instead of GCC for better sanitizer support
- Check that you're using a recent compiler version

### Linker Errors
If you encounter linker errors:
- Ensure all required libraries are installed
- Check that include paths are correct
- Verify that source files are properly listed in CMakeLists.txt

### Windows-Specific Issues
- Use forward slashes in paths for CMake
- Ensure Visual Studio is properly configured
- Consider using Clang on Windows for better fuzzing support

## Advanced Configuration

### Custom Compiler Flags
```bash
cmake .. -DCMAKE_C_FLAGS="-O2 -march=native"
```

### Custom Install Prefix
```bash
cmake .. -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --install .
```

### Disable Warnings
```bash
cmake .. -DCMAKE_C_FLAGS="-w"
```

## Verification

To verify the build succeeded:
```bash
# Check that fuzzers were created
ls -l dsl_parser_fuzzer jit_compiler_fuzzer stream_pipeline_fuzzer
ls -l distributed_protocol_fuzzer replication_fuzzer memory_pool_fuzzer

# Run a quick test
echo "test" | ./dsl_parser_fuzzer
```

## Continuous Integration

The project can be integrated with CI/CD systems:

### GitHub Actions
```yaml
name: Build and Fuzz
on: [push, pull_request]
jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Install dependencies
        run: sudo apt-get install build-essential cmake
      - name: Build
        run: |
          mkdir build && cd build
          cmake .. -DCMAKE_BUILD_TYPE=Debug
          cmake --build .
      - name: Fuzz
        run: |
          cd build
          timeout 60 ./dsl_parser_fuzzer ../fuzz/corpus/dsl_parser/
```

## Support

For issues or questions:
- Check the troubleshooting section above
- Review the main README.md
- Ensure all prerequisites are properly installed
