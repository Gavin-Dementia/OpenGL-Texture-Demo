cmake_minimum_required(VERSION 3.10)
project(OpenGLProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =========================
# Source files 
set(SRC_FILES
    src/main.cpp

    # GPU pipeline
    src/gpu/backend/RendererGPU.cpp
    src/gpu/backend/BindingManager.cpp
    src/gpu/upload/SceneUploader.cpp

    # buffers
    src/gpu/buffers/CounterBuffer.cpp
    src/gpu/buffers/DebugBuffer.cpp
    src/gpu/buffers/DrawBuffer.cpp
    src/gpu/buffers/InstanceBuffer.cpp
    src/gpu/buffers/MaterialBuffer.cpp
    src/gpu/buffers/MeshBoundsBuffer.cpp
    src/gpu/buffers/MeshBuffer.cpp
    src/gpu/buffers/ObjectBuffer.cpp
    src/gpu/buffers/SSBO.cpp
    src/gpu/buffers/TransformBuffer.cpp
    src/gpu/buffers/VisibilityBuffer.cpp

    #debug
    src/gpu/debug/GPUDebugValidator.cpp
    src/gpu/debug/PipelineStateTracker.cpp
    src/gpu/debug/BindingSanityChecker.cpp
    src/gpu/debug/BufferInspector.cpp

    #passes
    src/gpu/passes/DrawPass.cpp
    src/gpu/passes/VisibilityPass.cpp
    src/gpu/passes/CommandBuilderPass.cpp

    #pipeline
    src/gpu/pipeline/DrawPipeline.cpp
    src/gpu/pipeline/VisibilityPipeline.cpp

    # graphics
    src/graphics/Camera.cpp
    src/graphics/Shader.cpp
    src/graphics/Texture.cpp

    # scene
    src/scene/Mesh.cpp
    src/scene/MeshFactory.cpp
)

# =========================
# Glad (fixed path)
set(GLAD_SRC
    C:/openGL/glad/src/glad.c
)

# =========================
# Include directories
include_directories(
    C:/openGL/glad/include
    C:/openGL/glm
    C:/openGL/glfw-3.4.bin.WIN64/include
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)

# =========================
# Link directories
link_directories(
    C:/openGL/glfw-3.4.bin.WIN64/lib-mingw-w64
)

# =========================
# Executable
add_executable(main
    ${SRC_FILES}
    ${GLAD_SRC}
)

# =========================
# Libraries
target_link_libraries(main
    glfw3
    opengl32
    gdi32
)

# =========================
# Output path
set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
    VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

# =========================
# Copy DLL
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    C:/openGL/glfw-3.4.bin.WIN64/lib-mingw-w64/glfw3.dll
    ${PROJECT_SOURCE_DIR}/bin
)

