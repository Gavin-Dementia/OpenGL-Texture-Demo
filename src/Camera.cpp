#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 target, glm::vec3 worldup)
    : Position(position), WorldUp(worldup)
{
    glm::vec3 dir = glm::normalize(target - position);

    Pitch = glm::degrees(asin(dir.y));
    Yaw   = glm::degrees(atan2(dir.z, dir.x));

    updateCameraVectors();
}

Camera::Camera(glm::vec3 position, glm::vec3 worldup, float pitch, float yaw)
    : Position(position), WorldUp(worldup), Pitch(pitch), Yaw(yaw)
{
    Pitch = glm::clamp(Pitch, -89.0f, 89.0f);
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{  return glm::lookAt(Position, Position + Forward, Up);  }


void Camera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(Pitch)) * cos(glm::radians(Yaw));
    front.y = sin(glm::radians(Pitch));
    front.z = cos(glm::radians(Pitch)) * sin(glm::radians(Yaw));

    Forward = glm::normalize(front);
    Right   = glm::normalize(glm::cross(Forward, WorldUp));
    Up      = glm::normalize(glm::cross(Right, Forward));
}

void Camera::ProcessKeyboard(int direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == 0) Position += Forward * velocity;
    if (direction == 1) Position -= Forward * velocity;
    if (direction == 2) Position -= Right * velocity;
    if (direction == 3) Position += Right * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    if (constrainPitch)
        Pitch = glm::clamp(Pitch, -89.0f, 89.0f);

    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= yoffset;
    if (Zoom < 1.0f) Zoom = 1.0f;
    if (Zoom > 45.0f) Zoom = 45.0f;
}



