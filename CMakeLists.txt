cmake_minimum_required(VERSION 2.8.12)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0115 NEW)

project(fixpoint)

find_package(PkgConfig)

pkg_check_modules(CRYPT REQUIRED libcrypto++)
# find_package( libcryptopp REQUIRED)
include_directories(${CRYPT_INCLUDE_DIRS})
add_compile_options(${CRYPT_CFLAGS_OTHER})

find_package(Clang REQUIRED)
include_directories(SYSTEM ${CLANG_INCLUDE_DIRS})
set(CLANG_LIBS clangFrontend clangDriver clangSerialization clangParse clangCodeGen clangSema clangAnalysis clangEdit clangAST clangLex clangBasic)

find_package(LLVM REQUIRED)
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS}) 
add_definitions( ${LLVM_DEFINITIONS})
llvm_map_components_to_libnames( llvm_libs support irreader)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
FetchContent_Declare(
  wabt
  GIT_REPOSITORY https://github.com/fix-project/wabt
  GIT_TAG        fixpoint
  )
set(BUILD_TESTS off)
set(BUILD_LIBWASM off)
set(USE_INTERNAL_SHA256 on)
FetchContent_MakeAvailable(wabt)

FetchContent_Declare(
  absl
  GIT_REPOSITORY https://github.com/abseil/abseil-cpp
  GIT_TAG        20220623.0
  )
set(ABSL_PROPAGATE_CXX_STD on)
FetchContent_MakeAvailable(absl)

include(ExternalProject)
ExternalProject_Add(applications
  SOURCE_DIR "${PROJECT_SOURCE_DIR}/applications"
  INSTALL_COMMAND cmake -E echo "Skipping applications install."
  DEPENDS wat2wasm
  BUILD_ALWAYS true
  )

include(etc/build_type.cmake)
include(etc/cflags.cmake)
include(etc/clang_format.cmake)
include(etc/tests.cmake)

include_directories(SYSTEM "${CMAKE_BINARY_DIR}/_deps/wabt-src/include")
include_directories(SYSTEM "${CMAKE_BINARY_DIR}/_deps/wabt-build/include")

include_directories(SYSTEM "${CMAKE_BINARY_DIR}/_deps/absl-src")
include_directories(SYSTEM "${CMAKE_BINARY_DIR}/_deps/absl-build")

include_directories("${PROJECT_SOURCE_DIR}/src/util")
include_directories("${PROJECT_SOURCE_DIR}/src/runtime")
include_directories("${PROJECT_SOURCE_DIR}/src/component")
include_directories("${PROJECT_SOURCE_DIR}/src/wasm-rt")
include_directories("${PROJECT_SOURCE_DIR}/src/storage")

add_subdirectory("${PROJECT_SOURCE_DIR}/src/util")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/runtime")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/component")
add_subdirectory("${PROJECT_SOURCE_DIR}/src/wasm-rt")

set(FIXPOINT_LIBS runtime component util)
set(FIXPOINT_LIBS_TIMING runtime-timing component-timing util-timing)
set(FIXPOINT_LIBS_TIMING_2 runtime-timing-2 component-timing-2 util-timing-2)
#add_subdirectory("${PROJECT_SOURCE_DIR}/src/storage") # only headers

add_subdirectory("${PROJECT_SOURCE_DIR}/src/tester")

add_subdirectory("${PROJECT_SOURCE_DIR}/testing/wasm-examples")
add_subdirectory("${PROJECT_SOURCE_DIR}/testing/benchmark")

