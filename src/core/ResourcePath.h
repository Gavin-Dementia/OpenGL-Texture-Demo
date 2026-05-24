#pragma once
#include <string>

class ResourcePath
{
public:
    static std::string root()
    {
        return "C:/3Dproject/";   // need to config
    }

    static std::string shaders()
    {
        return root() + "shaders/";
    }

    static std::string shader(const std::string& name)
    {
        return shaders() + name;
    }    

    static std::string passShader(const std::string& name)
    {
        return shaders() + "passes/" + name;
    }    

    static std::string computeShader(const std::string& name)
    {
        return shaders() + "compute/" + name;
    }

    static std::string debugShader(const std::string& name)
    {
        return shaders() + "debug/" + name;
    }
};

