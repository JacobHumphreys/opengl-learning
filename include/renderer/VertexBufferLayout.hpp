#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <ostream>
#include <stdint.h>
#include <vector>
#include "print"

#include "debug.hpp"
#include "errors.hpp"

struct VertexBufferElement {
    uint32_t count;
    uint32_t type;
    uint8_t normalized;

    static uint32_t getSizeOfType(uint32_t type) {
        switch (type) {
        case GL_FLOAT:
            return sizeof(float);
        case GL_INT:
            return sizeof(int32_t);
        case GL_UNSIGNED_INT:
            return sizeof(uint32_t);
        case GL_UNSIGNED_BYTE:
            return sizeof(uint8_t);
        }
        ASSERT_BP(false);
    };
};

class VertexBufferLayout {
    std::vector<VertexBufferElement> elements;
    uint32_t stride;

public:
    VertexBufferLayout()
        : stride(0) { };

    template <typename T>
    void push([[maybe_unused]] uint32_t count) {
        static_assert(false);
    }

    template <>
    void push<float>(uint32_t count) {
        VertexBufferElement element = {
            .count = count,
            .type = GL_FLOAT,
            .normalized = GL_FALSE,
        };
        elements.push_back(element);
        stride += VertexBufferElement::getSizeOfType(elements[elements.size() - 1].type);
    }

    template <>
    void push<uint32_t>(uint32_t count) {
        elements.push_back({
            .count = count,
            .type = GL_UNSIGNED_INT,
            .normalized = GL_FALSE,
        });
        stride += VertexBufferElement::getSizeOfType(elements[elements.size() - 1].type);
    }

    template <>
    void push<uint8_t>(uint32_t count) {
        elements.push_back({
            .count = count,
            .type = GL_UNSIGNED_BYTE,
            .normalized = GL_FALSE,
        });
        stride += VertexBufferElement::getSizeOfType(elements[elements.size() - 1].type);
    }

    inline std::vector<VertexBufferElement> getElements() const {
        return this->elements;
    }

    inline uint32_t getStride() const {
        return this->stride;
    }
};
