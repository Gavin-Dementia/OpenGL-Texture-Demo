#include "gpu/debug/BindingSanityChecker.h"
#include <iostream>

void BindingSanityChecker::registerBinding(GLuint binding, GLuint buffer)
{
    bindingMap[binding] = buffer;
}

void BindingSanityChecker::validate(GLuint binding, const char* name)
{
    if (bindingMap.find(binding) == bindingMap.end())
    {
        std::cerr << "[GPU DEBUG] WARNING: " << name
                  << " binding not registered: " << binding << "\n";
        return;
    }

    if (bindingMap[binding] == 0)
    {
        std::cerr << "[GPU DEBUG] ERROR: " << name
                  << " buffer == 0\n";
    }
}

