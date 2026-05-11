#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// ============================================
// CPU-side camera controller
// (input + movement only)
// ============================================

class Camera
{
public:
    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldup);
    Camera(glm::vec3 position, glm::vec3 worldup, float pitch, float yaw = -90.0f);

    // ========================================
    // CPU interface
    void ProcessKeyboard(int direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMouseScroll(float yoffset);

    // ========================================
    // matrix generation (CPU fallback)
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(float aspectRatio) const;

    // ========================================
    // state
    glm::vec3 Position;
    glm::vec3 Forward;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Pitch = 0.0f;
    float Yaw = -90.0f;

    float MovementSpeed = 2.5f;
    float MouseSensitivity = 0.1f;
    float Zoom = 45.0f;

    // ========================================
    // GPU hook 
    // void updateGPUBuffer(); // write to UBO later

private:
    void updateCameraVectors();
};

#endif