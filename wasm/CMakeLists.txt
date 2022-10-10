cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0015 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(Aakara)

if(EMSCRIPTEN)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake")
    message(STATUS "EMSDK Path: $ENV{EMSDK}")
endif()

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../app/public)

# link_directories("lib/assimp/lib-js")
# link_libraries("assimp")

include_directories("./include")

# Assimp
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "Build all assimp importers" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "Build all assimp exporters" FORCE)
# set(PODOFO_BUILD_STATIC ON CACHE BOOL "Enable static build of PoDoFo")
set(USE_STLPORT OFF CACHE BOOL "Enable STL support for PoDoFo" FORCE)

set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "Enable .FBX import" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER OFF CACHE BOOL "Enable .OBJ import" FORCE)
set(ASSIMP_BUILD_STP_IMPORTER OFF CACHE BOOL "Enable .STP import" FORCE)
set(ASSIMP_BUILD_BLEND_IMPORTER OFF CACHE BOOL "Enable .blend import" FORCE)

set(ASSIMP_BUILD_FBX_EXPORTER ON CACHE BOOL "Enable FBX export" FORCE)
set(ASSIMP_BUILD_OBJ_EXPORTER ON CACHE BOOL "Enable OBJ export" FORCE)

add_subdirectory("./vendors/assimp-5.1.4")
add_subdirectory("./vendors/entt-3.9.0")
# add_subdirectory("./vendors/podofo-0.9.7")
add_subdirectory("./src/aakara")
