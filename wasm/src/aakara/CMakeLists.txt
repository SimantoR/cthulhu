cmake_minimum_required(VERSION 3.0.0)

set(AAKARA_SRC
    Camera.cpp
    fetch.cpp
    Global.cpp
    Lights.cpp
    Material.cpp
    Mesh.cpp
    Renderer.cpp
    Scene.cpp
    Shader.cpp
    Skybox.cpp
    Texture.cpp
    Transform.cpp
)

message(STATUS "Asset path: ${CMAKE_SOURCE_DIR}/assets")

add_library(aakara STATIC ${AAKARA_HEADERS} ${AAKARA_SRC})

target_compile_options(aakara PUBLIC -fexceptions)

target_include_directories(aakara PUBLIC "${PROJECT_SOURCE_DIR}/vendors/zlib")
target_include_directories(aakara PUBLIC "${PROJECT_SOURCE_DIR}/vendors/glm-0.9.9.8")
target_include_directories(aakara PUBLIC "${PROJECT_SOURCE_DIR}/include")

target_link_libraries(aakara assimp EnTT)
