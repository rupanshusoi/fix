add_custom_command(
  OUTPUT "addblob.wasm"
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/addblob.wat
          ${CMAKE_BINARY_DIR}/_deps/wabt-build/wat2wasm
  COMMAND ${CMAKE_BINARY_DIR}/_deps/wabt-build/wat2wasm
          --enable-multi-memory
          ${CMAKE_CURRENT_SOURCE_DIR}/addblob.wat
          -o addblob.wasm)

add_custom_target(addblob_wasm ALL DEPENDS addblob.wasm)

add_custom_command(
  OUTPUT "subblob.wasm"
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/subblob.wat
          ${CMAKE_BINARY_DIR}/_deps/wabt-build/wat2wasm
  COMMAND ${CMAKE_BINARY_DIR}/_deps/wabt-build/wat2wasm
          --enable-multi-memory
          ${CMAKE_CURRENT_SOURCE_DIR}/subblob.wat
          -o subblob.wasm)

add_custom_target(subblob_wasm ALL DEPENDS subblob.wasm)

add_custom_command(
  OUTPUT "add-simple.wasm"
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/add-simple.wat
          ${CMAKE_BINARY_DIR}/_deps/wabt-build/wat2wasm
  COMMAND ${CMAKE_BINARY_DIR}/_deps/wabt-build/wat2wasm
          --enable-multi-memory
          ${CMAKE_CURRENT_SOURCE_DIR}/add-simple.wat
          -o add-simple.wasm)

add_custom_target(add_simple_wasm ALL DEPENDS add-simple.wasm)

add_custom_command(
  OUTPUT "add-simple-8.wasm"
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/add-simple-8.wat
          ${CMAKE_BINARY_DIR}/_deps/wabt-build/wat2wasm
  COMMAND ${CMAKE_BINARY_DIR}/_deps/wabt-build/wat2wasm
          --enable-multi-memory
          ${CMAKE_CURRENT_SOURCE_DIR}/add-simple-8.wat
          -o add-simple-8.wasm)

add_custom_target(add_simple_8_wasm ALL DEPENDS add-simple-8.wasm)

add_custom_command(
  OUTPUT "fib.wasm"
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/fib.wat
          ${CMAKE_BINARY_DIR}/_deps/wabt-build/wat2wasm
  COMMAND ${CMAKE_BINARY_DIR}/_deps/wabt-build/wat2wasm
          --enable-multi-memory
          ${CMAKE_CURRENT_SOURCE_DIR}/fib.wat
          -o fib.wasm)

add_custom_target(fib_wasm ALL DEPENDS fib.wasm)

add_custom_command(
  OUTPUT "trap.wasm"
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/trap.wat
          ${CMAKE_BINARY_DIR}/_deps/wabt-build/wat2wasm
  COMMAND ${CMAKE_BINARY_DIR}/_deps/wabt-build/wat2wasm
          --enable-multi-memory
          ${CMAKE_CURRENT_SOURCE_DIR}/trap.wat
          -o trap.wasm)

add_custom_target(trap_wasm ALL DEPENDS trap.wasm)
