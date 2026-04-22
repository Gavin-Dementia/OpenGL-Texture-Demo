cmake_minimum_required(VERSION 3.10)
project(OpenGLProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_FILES
    src/main.cpp
    src/Camera.cpp

    src/core/Renderer.cpp

    src/graphics/Shader.cpp
    src/graphics/Texture.cpp

    src/objects/Cube.cpp
    src/objects/Mesh.cpp

    src/scene/Scene.cpp

    C:/openGL/glad/src/glad.c
)

include_directories(
    C:/openGL/glad/include
    C:/openGL/glm
    C:/openGL/glfw-3.4.bin.WIN64/include
    ${PROJECT_SOURCE_DIR}/include
)

link_directories(
    C:/openGL/glfw-3.4.bin.WIN64/lib-mingw-w64
)

add_executable(main ${SRC_FILES})

target_link_libraries(main
    glfw3
    opengl32
    gdi32
)

set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    C:/openGL/glfw-3.4.bin.WIN64/lib-mingw-w64/glfw3.dll
    ${PROJECT_SOURCE_DIR}/bin
)

