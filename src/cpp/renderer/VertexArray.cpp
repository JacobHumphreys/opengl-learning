#include <cstddef>
#include <cstdint>
#include <errors.hpp>
#include <renderer/VertexArray.hpp>
#include <renderer/VertexBuffer.hpp>
#include <renderer/VertexBufferLayout.hpp>

VertexArray::VertexArray() {
    CALL_GL(glGenVertexArrays(1, &this->renderer_id))
} VertexArray::~VertexArray() {
    CALL_GL(glDeleteVertexArrays(1, &this->renderer_id))
}

void VertexArray::addBuffer(VertexBuffer const& buffer, VertexBufferLayout const& layout) {
    bind();
    buffer.bind();
    const auto& elements = layout.getElements();

    uint32_t offset = 0;
    for (size_t i = 0; i < elements.size(); i++) {
        const auto& element = elements[i];

        CALL_GL(glEnableVertexAttribArray(i));
        // Sets ith index of vertex_array_object to bind to the currently bound buffer (vertex_buffer)
        CALL_GL(glVertexAttribPointer(
            i,
            element.count,
            element.type,
            element.normalized,
            layout.getStride() * element.count,
            reinterpret_cast<const void*>(offset)));
        offset += element.count * VertexBufferElement::getSizeOfType(element.type);
    }
}

void VertexArray::bind() const {
    CALL_GL(glBindVertexArray(this->renderer_id))
}

void VertexArray::unbind() const {
    CALL_GL(glBindVertexArray(0))
}
