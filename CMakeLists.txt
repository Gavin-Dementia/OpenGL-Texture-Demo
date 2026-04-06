cmake_minimum_required(VERSION 3.10)
project(OpenGLProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# === include path === need GLAD, GLM, GLFW
include_directories(
    C:/openGL/glad/include
    C:/openGL/glm
    C:/openGL/glfw-3.4.bin.WIN64/include
    ${PROJECT_SOURCE_DIR}/include
)

# === link library folders ===
link_directories(
    C:/openGL/glfw-3.4.bin.WIN64/lib-mingw-w64
)

# === source  ===
set(SRC_FILES
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/Shader.cpp
    C:/openGL/glad/include/glad/glad.c
)

# === build executable file ===
add_executable(main ${SRC_FILES})

# === linking library ===
target_link_libraries(main
    glfw3
    opengl32
    gdi32
)

# === 輸出 exe 到 bin ===
set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

# === 複製 DLL 到 bin ===
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    C:/openGL/glfw-3.4.bin.WIN64/lib-mingw-w64/glfw3.dll
    ${PROJECT_SOURCE_DIR}/bin
)

