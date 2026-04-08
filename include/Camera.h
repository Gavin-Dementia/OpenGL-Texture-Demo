#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Camera 
{
public:
    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldup);

    Camera(glm::vec3 position, glm::vec3 worldup, float pitch, float yaw= -90.0f);
    
    glm::mat4 GetViewMatrix();

    float Pitch;
    float Yaw;

    glm::vec3 Position;
    glm::vec3 Forward;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // camera operations
    float MovementSpeed = 2.5f;
    float MouseSensitivity = 0.1f;
    float Zoom = 45.0f;

    void ProcessKeyboard(int direction, float deltaTime); // 0: W, 1: S, 2: A, 3: D
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMouseScroll(float yoffset);
private:
    void updateCameraVectors();
};

#endif // CAMERA_H

