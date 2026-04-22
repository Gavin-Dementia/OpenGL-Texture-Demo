#include "RenderPasses.h"
#include "Renderer.h"
#include "Scene.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>

namespace RenderPasses {

void DepthPass(RenderContext& ctx, Renderer& renderer, Shader& depthShader)
{
    auto& scene = *ctx.scene;

    glm::vec3 lightDir = glm::normalize(scene.lights.dirLight.direction);

    glm::mat4 lightProj = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, 1.0f, 50.0f);
    glm::mat4 lightView = glm::lookAt(-lightDir * 20.0f, glm::vec3(0.0f), glm::vec3(0,1,0));

    ctx.lightSpaceMatrix = lightProj * lightView;

    glBindFramebuffer(GL_FRAMEBUFFER, ctx.depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    depthShader.use();
    depthShader.setMat4("lightSpaceMatrix", ctx.lightSpaceMatrix);

    renderer.executeDrawObjects(scene, depthShader);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ForwardPass(RenderContext& ctx, Renderer& renderer, Shader& shader)
{
    glViewport(0, 0, (int)ctx.width, (int)ctx.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderer.uploadCameraPublic(shader, *ctx.camera, ctx.width, ctx.height);
    renderer.uploadLightsPublic(*ctx.scene);

    shader.use();
    shader.setMat4("lightSpaceMatrix", ctx.lightSpaceMatrix);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ctx.depthMap);

    renderer.executeDrawObjects(*ctx.scene, shader);
}

void LightPass(Renderer& renderer, Scene& scene, Shader& lightShader, Camera& camera, float width, float height)
{
    lightShader.use();
    for (auto& obj : scene.lightVisuals)
    {
        lightShader.setMat4("modelLight", obj.getModel());
        obj.mesh->draw();
    }
}

} // namespace RenderPasses
