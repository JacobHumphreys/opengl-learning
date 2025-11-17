#include <cstdint>
#include "errors.hpp"
#include <renderer/IndexBuffer.hpp>
#include <stdint.h>


IndexBuffer::IndexBuffer(const void* data, uint32_t count)
    : count(count) {
    CALL_GL(glGenBuffers(1, &this->renderer_id));
    CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->renderer_id));
    CALL_GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW));
    CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

IndexBuffer::~IndexBuffer() {
    CALL_GL(glDeleteBuffers(1, &this->renderer_id));
}

void IndexBuffer::bind() const {
    CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->renderer_id));
}

void IndexBuffer::unbind() const {

    CALL_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
