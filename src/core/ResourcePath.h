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

    static std::string shaderVF(const std::string& name)
    {
        return shaders() + "passes/" + name;
    }    

    static std::string shaderC(const std::string& name)
    {
        return shaders() + "compute/" + name;
    }
};

