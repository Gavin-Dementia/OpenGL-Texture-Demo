#ifndef BindingSanityChecker_H
#define BindingSanityChecker_H
#include <glad/glad.h>
#include <unordered_map>

class BindingSanityChecker
{
public:
    void registerBinding(GLuint binding, GLuint buffer);
    void validate(GLuint binding, const char* name);

private:
    std::unordered_map<GLuint, GLuint> bindingMap;
};

#endif // BindingSanityChecker_H

