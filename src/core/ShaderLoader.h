#pragma once
#include <iostream>
#include "graphics/Shader.h"
#include "core/ResourcePath.h"

class ShaderLoader
{
public:

    static Shader loadPass( 
        const std::string& vs, 
        const std::string& fs)
    {
        std::string vPath =
            ResourcePath::passShader(vs);

        std::string fPath =
            ResourcePath::passShader(fs);

        std::cout << "[ShaderLoader] PASS VS: "
                  << vPath << std::endl;

        std::cout << "[ShaderLoader] PASS FS: "
                  << fPath << std::endl;

        return Shader(
            vPath.c_str(),
            fPath.c_str());
    }


    static Shader loadCompute(const std::string& cs)
    {
        std::string path = ResourcePath::computeShader(cs);
        std::cout << "[ShaderLoader] CS: " << path << std::endl;

        return Shader(path.c_str());
    }

    static Shader loadDebug(
        const std::string& vs,
        const std::string& fs)
    {
        std::string vPath =
            ResourcePath::debugShader(vs);

        std::string fPath =
            ResourcePath::debugShader(fs);

        std::cout << "[ShaderLoader] DEBUG VS: "
                  << vPath << std::endl;

        std::cout << "[ShaderLoader] DEBUG FS: "
                  << fPath << std::endl;

        return Shader(
            vPath.c_str(),
            fPath.c_str());
    }
};

