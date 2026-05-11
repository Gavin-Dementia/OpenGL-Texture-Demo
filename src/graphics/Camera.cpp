#include "graphics/Camera.h"

// ============================================
// constructor 1
Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldup)
    : Position(position), WorldUp(worldup)
{
    Forward = glm::normalize(target - position);
    Right   = glm::normalize(glm::cross(Forward, WorldUp));
    Up      = glm::normalize(glm::cross(Right, Forward));

    // derive yaw/pitch from forward vector
    Pitch = glm::degrees(asin(Forward.y));
    Yaw   = glm::degrees(atan2(Forward.z, Forward.x));

    updateCameraVectors();
}

// ============================================
// constructor 2
Camera::Camera(glm::vec3 position, glm::vec3 worldup, float pitch, float yaw)
    : Position(position), WorldUp(worldup), Pitch(pitch), Yaw(yaw)
{
    updateCameraVectors();
}

// ============================================
// View matrix
glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(Position, Position + Forward, Up);
}

// ============================================
// Projection matrix
glm::mat4 Camera::GetProjectionMatrix(float aspectRatio) const
{
    return glm::perspective(glm::radians(Zoom), aspectRatio, 0.1f, 1000.0f);
}

// ============================================
// Keyboard movement
// 0: W, 1: S, 2: A, 3: D
void Camera::ProcessKeyboard(int direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;

    if (direction == 0) Position += Forward * velocity;
    if (direction == 1) Position -= Forward * velocity;
    if (direction == 2) Position -= Right * velocity;
    if (direction == 3) Position += Right * velocity;
}

// ============================================
// Mouse movement
void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    if (constrainPitch)
    {
        if (Pitch > 89.0f) Pitch = 89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;
    }

    updateCameraVectors();
}

// ============================================
// Scroll zoom
void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= yoffset;
    if (Zoom < 1.0f) Zoom = 1.0f;
    if (Zoom > 90.0f) Zoom = 90.0f;
}

// ============================================
// update vectors
void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

    Forward = glm::normalize(front);
    Right   = glm::normalize(glm::cross(Forward, WorldUp));
    Up      = glm::normalize(glm::cross(Right, Forward));
}