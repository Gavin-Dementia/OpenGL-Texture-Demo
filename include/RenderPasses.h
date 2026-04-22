#ifndef RENDERPASSES_H
#define RENDERPASSES_H

#include <glm/glm.hpp>

class Renderer;
class Scene;
class Shader;
class Camera;

namespace RenderPasses {
    // Render depth (shadow) pass.
    void DepthPass(RenderContext& ctx, Renderer& renderer, Shader& depthShader)

    // Forward pass that renders the scene from the camera, sampling the provided lightSpaceMatrix and depth map.
    void ForwardPass(RenderContext& ctx, Renderer& renderer, Shader& shader)

    // Render light visuals / light pass
    void LightPass(Renderer& renderer, Scene& scene, Shader& lightShader, Camera& camera, float width, float height);
}

#endif // RENDERPASSES_H

