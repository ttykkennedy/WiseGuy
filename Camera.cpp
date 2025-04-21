#include "Camera.h"

float Camera::s_cameraX = 0.0f;
float Camera::s_cameraY = 0.0f;
float Camera::s_cameraZoom = 1.0f;

void Camera::SetPosition(float x, float y)
{
    s_cameraX = x;
    s_cameraY = y;
}

float Camera::GetPositionX()
{
    return s_cameraX;
}

float Camera::GetPositionY()
{
    return s_cameraY;
}

void Camera::SetZoom(float zoom_factor)
{
    s_cameraZoom = zoom_factor;
}

float Camera::GetZoom()
{
    return s_cameraZoom;
}

glm::ivec2 Camera::GetCameraDimensions()
{
    return glm::ivec2(640, 360);
}
