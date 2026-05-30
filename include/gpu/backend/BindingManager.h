#ifndef BINDING_MANAGER_H
#define BINDING_MANAGER_H

#include <glad/glad.h>

class SceneUploader;

class BindingManager
{
public:

    void bindFrame(const SceneUploader& scene);

    void bindForCompute(const SceneUploader& scene);
    void bindForDraw(const SceneUploader& scene);

    void reset();

    // debug hook
    bool validateBindings() const;

private:
    void bindSSBO(GLuint binding, GLuint buffer);

    // cache for debug
    GLuint bound[12] = {0};
};

#endif  // BINDING_MANAGER_H

