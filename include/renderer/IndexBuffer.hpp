#pragma once

#include <cstdint>
#include <stdint.h>
class IndexBuffer {
    uint32_t renderer_id;
    uint32_t count;

public:
    IndexBuffer(const void* data, uint32_t count);

    ~IndexBuffer();
    void bind() const;
    void unbind() const;

    uint32_t getCount() const {
        return count;
    }
};
