
# defines link_libc
include(../util/cmake_helper_functions.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Os -mreference-types -I/home/rsoi/fix/applications/test/cereal/include -I ~/wasm-toolchain/wasix/include")
set(CMAKE_CXX_COMPILER $ENV{HOME}/wasm-toolchain/sysroot/bin/clang++)

add_link_options( -Wl,--no-entry -Wl,--allow-undefined, -Wl, -Werror, -Wunused-parameter, -fno-exceptions)

add_library(test_obj OBJECT "test.cc")

link_libc("${CMAKE_BINARY_DIR}/test/CMakeFiles/test_obj.dir/test.cc.o" test_obj test.wasm) # outputs test.wasm

add_custom_command(
        OUTPUT "test-linked-memory.wasm"
        DEPENDS ${CMAKE_BINARY_DIR}/_deps/wasm-tools-build/src/module-combiner/wasmlink
        test.wasm
        util.wasm
        COMMAND ${CMAKE_BINARY_DIR}/_deps/wasm-tools-build/src/module-combiner/wasmlink
        --enable-multi-memory
        --enable-exceptions
        test.wasm
        util.wasm
        -n util
        -o "test-linked-memory.wasm"
)

# link fixpoint_storage, which defines wat tables and memories
add_custom_command(
        OUTPUT "linked_with_storage.wasm"
        DEPENDS ${CMAKE_BINARY_DIR}/_deps/wasm-tools-build/src/module-combiner/wasmlink
        test-linked-memory.wasm
        ../util/fixpoint_storage.wasm
        COMMAND ${CMAKE_BINARY_DIR}/_deps/wasm-tools-build/src/module-combiner/wasmlink
        --enable-multi-memory
        --enable-exceptions
        test-linked-memory.wasm
        ../util/fixpoint_storage.wasm
        -n fixpoint_storage
        -o "linked_with_storage.wasm"
)

# link wasi_snapshot_preview1, which defines stubs for fd_write/seek/close
add_custom_command(
        OUTPUT "test-linked.wasm"
        DEPENDS ${CMAKE_BINARY_DIR}/_deps/wasm-tools-build/src/module-combiner/wasmlink
        linked_with_storage.wasm
        ../util/wasi_snapshot_preview1.wasm
        COMMAND ${CMAKE_BINARY_DIR}/_deps/wasm-tools-build/src/module-combiner/wasmlink
        --enable-multi-memory
        --enable-exceptions
        linked_with_storage.wasm
        ../util/wasi_snapshot_preview1.wasm
        -n wasi_snapshot_preview1
        -o "test-linked.wasm"
)

add_custom_target(
        test-test ALL
        DEPENDS test-linked.wasm
        )
