#include "graphics/Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

// =======================
// read file safely
std::string Shader::readFile(const char* path)
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        std::cerr << "[Shader ERROR] Cannot open file: " << path << std::endl;
        throw std::runtime_error("Shader file not found");
    }

    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// =======================
// compile shader
unsigned int Shader::compileShader(unsigned int type, const std::string& src)
{
    unsigned int shader = glCreateShader(type);

    const char* code = src.c_str();
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);

    checkError(shader, "SHADER");

    return shader;
}

// =======================
// link program
void Shader::linkProgram(unsigned int v, unsigned int f)
{
    ID = glCreateProgram();

    glAttachShader(ID, v);
    glAttachShader(ID, f);
    glLinkProgram(ID);

    checkError(ID, "PROGRAM");

    glDeleteShader(v);
    glDeleteShader(f);
}

// =======================
// constructor (vertex + fragment)
Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::string vSrc = readFile(vertexPath);
    std::string fSrc = readFile(fragmentPath);

    unsigned int v = compileShader(GL_VERTEX_SHADER, vSrc);
    unsigned int f = compileShader(GL_FRAGMENT_SHADER, fSrc);

    linkProgram(v, f);

    std::cout << "[Shader] Loaded OK: " << vertexPath << std::endl;
}

// =======================
// compute shader
Shader::Shader(const char* computePath)
{
    std::string src = readFile(computePath);

    unsigned int c = compileShader(GL_COMPUTE_SHADER, src);

    ID = glCreateProgram();
    glAttachShader(ID, c);
    glLinkProgram(ID);

    checkError(ID, "PROGRAM");

    glDeleteShader(c);
}

// =======================
void Shader::use() const
{  glUseProgram(ID);  }

// =======================
// uniform setters
void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// =======================
// error check
void Shader::checkError(unsigned int obj, std::string type)
{
    int success;
    char infoLog[1024];

    if (type == "SHADER")
    {
        glGetShaderiv(obj, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            glGetShaderInfoLog(obj, 1024, nullptr, infoLog);
            std::cerr << "[Shader Compile Error]\n" << infoLog << std::endl;
            throw std::runtime_error("Shader compile failed");
        }
    }
    else
    {
        glGetProgramiv(obj, GL_LINK_STATUS, &success);

        if (!success)
        {
            glGetProgramInfoLog(obj, 1024, nullptr, infoLog);
            std::cerr << "[Program Link Error]\n" << infoLog << std::endl;
            throw std::runtime_error("Program link failed");
        }
    }
}