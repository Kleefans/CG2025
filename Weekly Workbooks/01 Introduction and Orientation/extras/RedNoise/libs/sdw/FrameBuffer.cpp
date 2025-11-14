#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(size_t w, size_t h) : width(w), height(h), depthBuffer(w * h, std::numeric_limits<float>::lowest()) {}

void FrameBuffer::clearDepth() {
    std::fill(depthBuffer.begin(), depthBuffer.end(), std::numeric_limits<float>::lowest());
}

float FrameBuffer::getDepth(size_t x, size_t y) const {
    return depthBuffer[x + y * width];
}

void FrameBuffer::setDepth(size_t x, size_t y, float depth) {
    depthBuffer[x + y * width] = depth;
}

bool FrameBuffer::testAndSetDepth(size_t x, size_t y, float depth) {
    size_t idx = x + y * width;
    if (depth > depthBuffer[idx]) {
        depthBuffer[idx] = depth;
        return true;
    }
    return false;
}