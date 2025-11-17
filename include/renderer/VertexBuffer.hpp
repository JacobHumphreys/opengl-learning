#pragma once

#include <cstdint>
#include <stdint.h>
class VertexBuffer {
    uint32_t renderer_id;

public:
    VertexBuffer(const void* data, uint32_t size);

    ~VertexBuffer();
    void bind() const;
    void unbind() const;
};
