#pragma once
#include <string>

class ResourcePath
{
public:
    static std::string root()
    {
        return "C:/3Dproject/";   // ⚠️ 先寫死，之後再做 config
    }

    static std::string shaders()
    {
        return root() + "shaders/";
    }

    static std::string shader(const std::string& name)
    {
        return shaders() + name;
    }
};

