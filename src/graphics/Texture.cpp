#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Texture.h"

Texture::Texture(const std::string& path)
{
    stbi_set_flip_vertically_on_load(true); // 翻轉Y軸，符合OpenGL習慣
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    
    // 設置默認過濾和環繞方式
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Texture loaded successfully: " << path << std::endl;
    }
    else
    std::cout << "Failed to load texture: " << path << std::endl;
    
    stbi_image_free(data);
}

Texture::~Texture(){  glDeleteTextures(1, &ID);  }

void Texture::bind(unsigned int slot = 0) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, ID);
}

unsigned int Texture::getID() const {  return ID;  }

