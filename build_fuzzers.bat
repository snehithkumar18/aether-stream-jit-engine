@echo off
REM Build script for fuzzing harnesses with libFuzzer on Windows

echo Building Aether Stream JIT Engine fuzzers...

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Debug ^
         -DCMAKE_C_FLAGS="/fsanitize=fuzzer,address,undefined /O1 /Zi" ^
         -DCMAKE_EXE_LINKER_FLAGS="/fsanitize=fuzzer,address,undefined"

REM Build all fuzzers
cmake --build . --config Debug

echo Build complete. Fuzzers are now available in build\Debug\
echo.
echo To run a fuzzer:
echo   build\Debug\dsl_parser_fuzzer.exe fuzz\corpus\dsl_parser\
echo   build\Debug\jit_compiler_fuzzer.exe fuzz\corpus\jit_compiler\
echo   build\Debug\stream_pipeline_fuzzer.exe fuzz\corpus\stream_pipeline\
echo   build\Debug\distributed_protocol_fuzzer.exe fuzz\corpus\distributed_protocol\
echo   build\Debug\replication_fuzzer.exe fuzz\corpus\replication\
echo   build\Debug\memory_pool_fuzzer.exe fuzz\corpus\memory_pool\
