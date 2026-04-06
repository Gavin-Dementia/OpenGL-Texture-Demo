#include "Shader.h"
#include <exception>

#define GL_STATIC
#include <glad/glad.h>
#include <GLFW/glfw3.h>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::stringstream vShaderStream, fShaderStream;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // open files
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        if(!vShaderFile.is_open() || !fShaderFile.is_open())
            throw std::exception();

        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // convert stream into string
        vertexString   = vShaderStream.str();
        fragmentString = fShaderStream.str();	
        
        vertexSource = vertexString.c_str();
        fragmentSource = fragmentString.c_str();      
        
        // std::cout << vertexSource << std::endl;
        // std::cout << fragmentSource << std::endl;
        
        // close file handlers
        // vShaderFile.close();
        // fShaderFile.close();	
    }
    catch(std::exception& e)
    {
        std::cout << "Failed to open shader files" << std::endl;
    }

    // 2. compile shaders
    unsigned int vertex, fragment;

    vertex= glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    fragment= glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    ID= glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);

}

// === 使用 shader ===
void Shader::use()
{  glUseProgram(ID);  }

// === uniform 工具 ===
void Shader::setBool(const std::string &name, bool value) const
{  glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);  }

void Shader::setInt(const std::string &name, int value) const
{  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);  }

void Shader::setFloat(const std::string &name, float value) const
{  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);  }

void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if(type != "PROGRAM")
    {
        glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(ID, 1024, NULL, infoLog);
            std::cout << "ERROR: " << infoLog << std::endl;
        }
        else return;
    }
    else
    {
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(ID, 1024, NULL, infoLog);
            std::cout << "ERROR: " << infoLog << std::endl;
        }
        else return;
    }
}

