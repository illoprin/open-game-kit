#include "vertex_array.hpp"
#include "resource.hpp"

void VertexArray::SetAttribute(
  const Buffer&                 b,
  const std::vector<Attribute>& attrs
) noexcept {
  if (!id) return;
  Bind();
  glBindBuffer(b.Target(), b.ID());
  for (const auto& attr : attrs) {
    glEnableVertexAttribArray(attr.Location);
    if (attr.Type == GL_FLOAT) {
      glVertexAttribPointer(
        attr.Location,
        attr.Comps,
        attr.Type,
        attr.Normalized ? GL_TRUE : GL_FALSE,
        attr.Stride,
        (const void*)attr.Offset
      );
    } else {
      glVertexAttribIPointer(
        attr.Location,
        attr.Comps,
        attr.Type,
        attr.Stride,
        (const void*)attr.Offset
      );
    }
    if (attr.Divisor != 0) glVertexAttribDivisor(attr.Location, attr.Divisor);
  }
  Unbind();
}

void VertexArray::AttachIndexBuffer(const Buffer& buf) noexcept {
  if (!id) return;
  if (buf.Target() != GL_ELEMENT_ARRAY_BUFFER) return;

  Bind();
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf.ID());
  eboBinding = buf.ID();
  Unbind();
}
