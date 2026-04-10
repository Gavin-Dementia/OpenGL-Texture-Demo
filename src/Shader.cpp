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
    
// activate the shader
// ------------------------------------------------------------------------
void Shader::use() const
{  glUseProgram(ID);  }
// utility uniform functions
// ------------------------------------------------------------------------
void Shader::setBool(const std::string &name, bool value) const
{  glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);  }
// ------------------------------------------------------------------------
void Shader::setInt(const std::string &name, int value) const
{  glUniform1i(glGetUniformLocation(ID, name.c_str()), value);  }
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string &name, float value) const
{  glUniform1f(glGetUniformLocation(ID, name.c_str()), value);  }
// ------------------------------------------------------------------------
void Shader::setVec2(const std::string &name, const glm::vec2 &value) const
{  glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);  }
void Shader::setVec2(const std::string &name, float x, float y) const
{  glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);  }
// ------------------------------------------------------------------------
void Shader::setVec3(const std::string &name, const glm::vec3 &value) const
{  glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);  }
void Shader::setVec3(const std::string &name, float x, float y, float z) const
{  glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);  }
// ------------------------------------------------------------------------
void Shader::setVec4(const std::string &name, const glm::vec4 &value) const
{  glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);  }
void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const
{  glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);  }
// ------------------------------------------------------------------------
void Shader::setMat2(const std::string &name, const glm::mat2 &mat) const
{  glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);  }
// ------------------------------------------------------------------------
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const
{  glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);  }
// ------------------------------------------------------------------------
void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const
{  glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);  }

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
            std::cout << ID << "ERROR: " << infoLog << std::endl;
        }
        else return;
    }
}

