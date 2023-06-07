cmake_minimum_required(VERSION 3.10)

project(client)

include_directories(BEFORE SYSTEM include)
include_directories(BEFORE .)
link_directories(${CMAKE_SOURCE_DIR}/lib)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(opt_and_debug_compiler_options
  # optimisations
  -O0
  # debugging
  -g3
)
set(opt_and_debug_linker_options
  # optimisations
  -O0
  # debugging
  -g3
  --profiling-funcs
  -sDEMANGLE_SUPPORT=1
  # ensure fast link in debug mode: https://emscripten.org/docs/optimizing/Optimizing-Code.html#link-times
  -sERROR_ON_WASM_CHANGES_AFTER_LINK
)

# exception handling disabled for emscripten
set(exception_compile_definitions
  DISABLE_EXCEPTION_THROWING
  DISABLE_EXCEPTION_CATCHING
)
set(exception_compile_options
  -fignore-exceptions
  -fno-exceptions
)
set(exception_link_options
  -fno-except
  -fignore-exceptions
)

add_compile_definitions(
  GLFW_INCLUDE_ES3
  ${exception_compile_definitions}
)

add_executable(client
  # project-specific:
  main.cpp
  log_window.cpp
  render_window.cpp
  # 3rd party libraries:
  include/imgui/imgui.cpp
  include/imgui/imgui_draw.cpp
  include/imgui/imgui_tables.cpp
  include/imgui/imgui_widgets.cpp
  include/imgui/imgui_impl_glfw.cpp
  include/imgui/imgui_stdlib.cpp
  include/imgui/imgui_impl_opengl3.cpp
)

target_link_libraries(client
)

target_compile_options(client PRIVATE
  ${opt_and_debug_compiler_options}
  -msse
  -msse2
  -msse3
  -mssse3
  -msse4.1
  -msse4.2
  -mavx
  -msimd128
  # emscripten ports
  -sUSE_BOOST_HEADERS=1
  ${exception_compile_options}
  # warnings
  -Wall
  -Wextra
)
# suppress warnings for external libraries built as part of include
#file(GLOB_RECURSE include_files include/*)
#set_source_files_properties(${include_files} PROPERTIES COMPILE_FLAGS "-w")

target_link_options(client PRIVATE
  ${opt_and_debug_linker_options}
  -lwebsocket.js
  --bind
  -sALLOW_MEMORY_GROWTH=1
  -sWASM_BIGINT
  -sELIMINATE_DUPLICATE_FUNCTIONS=1
  -sUSE_GLFW=3
  -sMIN_WEBGL_VERSION=2
  -sMAX_WEBGL_VERSION=2
  -sMIN_WEBGL_VERSION=2
  ${exception_link_options}
  -sEXPORTED_RUNTIME_METHODS=[ccall]
  # don't emit code for node.js etc
  -sENVIRONMENT=web
  --shell-file=${CMAKE_SOURCE_DIR}/html/client.html
)

set(CMAKE_EXECUTABLE_SUFFIX ".html")
