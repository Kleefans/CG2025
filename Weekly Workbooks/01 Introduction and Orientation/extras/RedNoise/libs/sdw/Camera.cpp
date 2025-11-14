#include "Camera.h"
#include <cmath>

Camera::Camera(glm::vec3 pos, float focal, float scale, size_t vpW, size_t vpH)
    : position(pos), focalLength(focal), imageScaleFactor(scale), viewportWidth(vpW), viewportHeight(vpH) {}

CanvasPoint Camera::project(const glm::vec3 &vertexPosition) const {
    glm::vec3 vertexRelativePosition = vertexPosition - position;
    float x = vertexRelativePosition.x;
    float y = vertexRelativePosition.y;
    float z = vertexRelativePosition.z;

    float u = -focalLength * (x / z) * imageScaleFactor + (float)viewportWidth / 2.0f;
    float v =  focalLength * (y / z) * imageScaleFactor + (float)viewportHeight / 2.0f;
    float zDepth = 1.0f / -z;

    return CanvasPoint(round(u), round(v), zDepth);
}