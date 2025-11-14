#pragma once

#include <cstddef>
#include <vector>

class FrameBuffer {
public:
    size_t width;
    size_t height;

private:
    std::vector<float> depthBuffer;

public:
    FrameBuffer(size_t w, size_t h);

    void clearDepth();
    float getDepth(size_t x, size_t y) const;
    void setDepth(size_t x, size_t y, float depth);
    bool testAndSetDepth(size_t x, size_t y, float depth);
};