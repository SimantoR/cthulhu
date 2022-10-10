cmake_minimum_required(VERSION 3.0.0)

file(GLOB_RECURSE CORE_HDR "*.hpp")
file(GLOB_RECURSE CORE_SRC "*.cpp")

set(EMCC_FLAGS "--bind")
# Set a memory limit
set(EMCC_FLAGS "${EMCC_FLAGS} -s ENVIRONMENT=web,worker")
set(EMCC_FLAGS "${EMCC_FLAGS} -s FETCH=1")
set(EMCC_FLAGS "${EMCC_FLAGS} -s WASM=1")
set(EMCC_FLAGS "${EMCC_FLAGS} -s EXPORT_NAME='loadaakara'")
set(EMCC_FLAGS "${EMCC_FLAGS} -s MODULARIZE=1")
# set(EMCC_FLAGS "${EMCC_FLAGS} -s EXPORT_ES6=1")
set(EMCC_FLAGS "${EMCC_FLAGS} -s DEMANGLE_SUPPORT=1")
set(EMCC_FLAGS "${EMCC_FLAGS} -s TOTAL_MEMORY=524288000")
set(EMCC_FLAGS "${EMCC_FLAGS} -s USE_PTHREADS=1")
set(EMCC_FLAGS "${EMCC_FLAGS} -s ALLOW_BLOCKING_ON_MAIN_THREAD=1")
# set(EMCC_FLAGS "${EMCC_FLAGS} -s PROXY_TO_PTHREAD=1")
set(EMCC_FLAGS "${EMCC_FLAGS} -s PTHREAD_POOL_SIZE=4")
set(EMCC_FLAGS "${EMCC_FLAGS} -s ASSERTIONS=1")
set(EMCC_FLAGS "${EMCC_FLAGS} -s GL_ASSERTIONS=1")
set(EMCC_FLAGS "${EMCC_FLAGS} -s SINGLE_FILE=0")
set(EMCC_FLAGS "${EMCC_FLAGS} -s GL_DEBUG=1")
set(EMCC_FLAGS "${EMCC_FLAGS} -s OFFSCREEN_FRAMEBUFFER=0")
set(EMCC_FLAGS "${EMCC_FLAGS} -s NO_DISABLE_EXCEPTION_CATCHING=1")

# Load assets as a single file for WASM
set(EMCC_FLAGS "${EMCC_FLAGS} --preload-file ${CMAKE_SOURCE_DIR}/assets@/")

set(EMCC_FLAGS "${EMCC_FLAGS} -O3")

message(STATUS "Asset path: ${CMAKE_SOURCE_DIR}/assets")

add_executable(aakara ${CORE_SRC} ${CORE_HDR})
set_target_properties(aakara PROPERTIES LINK_FLAGS ${EMCC_FLAGS})
target_compile_options(aakara PUBLIC -fexceptions)

target_include_directories(aakara PUBLIC "${PROJECT_SOURCE_DIR}/vendors/glm-0.9.9.8")
target_include_directories(aakara PUBLIC "${PROJECT_SOURCE_DIR}/include")

if (${EMSCRIPTEN})
    target_compile_options(assimp PUBLIC -fexceptions)
    target_compile_options(assimp PUBLIC -pthread)
endif()

target_link_libraries (aakara assimp EnTT)
