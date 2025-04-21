#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"

class Camera
{
public:
    static void SetPosition(float x, float y);

    static float GetPositionX();

    static float GetPositionY();

    static void SetZoom(float zoom_factor);

    static float GetZoom();

    static glm::ivec2 GetCameraDimensions();

private:
    static float s_cameraX;
    static float s_cameraY;
    static float s_cameraZoom;
};

#endif
