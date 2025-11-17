#include <cstdint>
#include "errors.hpp"
#include <renderer/VertexBuffer.hpp>

void VertexBuffer::bind() const {
    CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, this->renderer_id));
}

void VertexBuffer::unbind() const {

    CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

VertexBuffer::VertexBuffer(const void* data, uint32_t size) {
    CALL_GL(glGenBuffers(1, &this->renderer_id));
    CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, this->renderer_id));
    CALL_GL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
    CALL_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

VertexBuffer::~VertexBuffer() {
    CALL_GL(glDeleteBuffers(1, &this->renderer_id));
}
