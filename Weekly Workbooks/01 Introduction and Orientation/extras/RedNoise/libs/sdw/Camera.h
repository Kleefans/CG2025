#pragma once

#include <glm/glm.hpp>
#include "CanvasPoint.h"

class Camera {
public:
    glm::vec3 position;
    float focalLength;
    float imageScaleFactor;
    size_t viewportWidth;
    size_t viewportHeight;

    Camera(glm::vec3 pos, float focal, float scale, size_t vpW, size_t vpH);

    CanvasPoint project(const glm::vec3 &vertexPosition) const;
};