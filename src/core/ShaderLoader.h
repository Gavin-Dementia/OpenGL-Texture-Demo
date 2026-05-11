#pragma once
#include <iostream>
#include "graphics/Shader.h"
#include "core/ResourcePath.h"

class ShaderLoader
{
public:

    static Shader load(const std::string& vs, const std::string& fs)
    {
        std::string vPath = ResourcePath::shader(vs);
        std::string fPath = ResourcePath::shader(fs);

        std::cout << "[ShaderLoader] VS: " << vPath << std::endl;
        std::cout << "[ShaderLoader] FS: " << fPath << std::endl;

        return Shader(vPath.c_str(), fPath.c_str());
    }

    static Shader loadCompute(const std::string& cs)
    {
        std::string path = ResourcePath::shader(cs);
        std::cout << "[ShaderLoader] CS: " << path << std::endl;

        return Shader(path.c_str());
    }
};

