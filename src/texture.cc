#include "texture.hpp"
#include "resource.hpp"

// ----------------------------------------------------------------------
//                       Texture Format
// ----------------------------------------------------------------------

TextureFormatInfo GetTextureFormatInfo(GLenum internalFormat) {
  switch (internalFormat) {
    // RGBA formats
    case GL_RGBA8:
    case GL_SRGB8_ALPHA8:
      return {GL_UNSIGNED_BYTE, GL_RGBA};
    case GL_RGBA16F:
      return {GL_HALF_FLOAT, GL_RGBA};
    case GL_RGBA32F:
      return {GL_FLOAT, GL_RGBA};

    // RGB formats
    case GL_RGB8:
    case GL_SRGB8:
      return {GL_UNSIGNED_BYTE, GL_RGB};
    case GL_RGB16F:
      return {GL_HALF_FLOAT, GL_RGB};
    case GL_RGB32F:
      return {GL_FLOAT, GL_RGB};

    // Depth formats
    case GL_DEPTH_COMPONENT16:
    case GL_DEPTH_COMPONENT24:
    case GL_DEPTH_COMPONENT32F:
      return {GL_FLOAT, GL_DEPTH_COMPONENT};
    case GL_DEPTH24_STENCIL8:
      return {GL_UNSIGNED_INT_24_8, GL_DEPTH_STENCIL};

    // Single channel
    case GL_R8:
      return {GL_UNSIGNED_BYTE, GL_RED};
    case GL_R16F:
      return {GL_HALF_FLOAT, GL_RED};
    case GL_R32F:
      return {GL_FLOAT, GL_RED};

    default:
      if (internalFormat >= GL_DEPTH_COMPONENT
          && internalFormat <= GL_DEPTH_STENCIL) {
        return {GL_FLOAT, GL_DEPTH_COMPONENT};
      }
      return {GL_UNSIGNED_BYTE, GL_RGBA};
  }
}

// ----------------------------------------------------------------------
//                       Texture
// ----------------------------------------------------------------------

// Texture implementation
Texture::Texture()
    : Resource(), target(0), width(0), height(0), depth(0), internalFormat(0) {
  glGenTextures(1, &id);
}

Texture::~Texture() {
  if (id != 0) {
    glDeleteTextures(1, &id);
    id = 0;
  }
}

Texture::Texture(Texture&& other) noexcept
    : Resource(std::move(other)),
      target(other.target),
      width(other.width),
      height(other.height),
      depth(other.depth),
      internalFormat(other.internalFormat) {
  other.target         = 0;
  other.width          = 0;
  other.height         = 0;
  other.depth          = 0;
  other.internalFormat = 0;
}

Texture& Texture::operator =(Texture&& other) noexcept {
  if (this != &other) {
    if (id != 0) { glDeleteTextures(1, &id); }
    Resource::operator =(std::move(other));
    target               = other.target;
    width                = other.width;
    height               = other.height;
    depth                = other.depth;
    internalFormat       = other.internalFormat;
    other.target         = 0;
    other.width          = 0;
    other.height         = 0;
    other.depth          = 0;
    other.internalFormat = 0;
  }
  return *this;
}

GLuint Texture::Target() const {
  return target;
}

GLint Texture::Width() const {
  return width;
}

GLint Texture::Height() const {
  return height;
}

GLint Texture::Depth() const {
  return depth;
}

GLenum Texture::InternalFormat() const {
  return internalFormat;
}

void Texture::Bind(GLuint unit) const {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(target, id);
}

void Texture::GenerateMipmaps() const {
  Bind();
  glGenerateMipmap(target);
}

void Texture::SetSamplerState(
  GLenum wrapMode,
  GLenum magFilter,
  GLenum minFilter
) const {
  Bind();
  glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapMode);
  glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapMode);

  if (target == GL_TEXTURE_3D || target == GL_TEXTURE_CUBE_MAP) {
    glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapMode);
  }

  glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
  glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
}

void Texture::SetParameter(GLenum pname, GLint param) const {
  Bind();
  glTexParameteri(target, pname, param);
}

void Texture::SetParameterf(GLenum pname, GLfloat param) const {
  Bind();
  glTexParameterf(target, pname, param);
}

// ----------------------------------------------------------------------
//                       Texture2D
// ----------------------------------------------------------------------

Texture2D::Texture2D() {
  target = GL_TEXTURE_2D;
}

void Texture2D::FromImage(const Image2D& image, GLenum internalFmt) {
  if (image.Width() == 0 || image.Height() == 0) return;

  width          = image.Width();
  height         = image.Height();
  depth          = 1;
  internalFormat = internalFmt;

  auto [sourceType, format] = GetTextureFormatInfo(internalFmt);

  Bind();
  glTexImage2D(
    target,
    0,
    internalFormat,
    width,
    height,
    0,
    format,
    sourceType,
    image.Pix()
  );

  SetSamplerState(GL_REPEAT, GL_LINEAR, GL_LINEAR);
}

void Texture2D::FromData(
  const uch* data,
  GLint      w,
  GLint      h,
  GLenum     internalFmt
) {
  if (w == 0 || h == 0) return;

  width          = w;
  height         = h;
  depth          = 1;
  internalFormat = internalFmt;

  auto [sourceType, format] = GetTextureFormatInfo(internalFmt);

  Bind();
  glTexImage2D(
    target,
    0,
    internalFormat,
    width,
    height,
    0,
    format,
    sourceType,
    data
  );
}

void Texture2D::Allocate(GLint w, GLint h, GLenum internalFmt) {
  width          = w;
  height         = h;
  depth          = 1;
  internalFormat = internalFmt;

  auto [sourceType, format] = GetTextureFormatInfo(internalFmt);

  Bind();
  glTexImage2D(
    target,
    0,
    internalFormat,
    width,
    height,
    0,
    format,
    sourceType,
    nullptr
  );

  SetSamplerState(GL_REPEAT, GL_LINEAR, GL_LINEAR);
}

void Texture2D::SetSubImage(
  GLint       x,
  GLint       y,
  GLint       w,
  GLint       h,
  GLenum      format,
  GLenum      type,
  const void* data
) {
  Bind();
  glTexSubImage2D(target, 0, x, y, w, h, format, type, data);
}

// ----------------------------------------------------------------------
//                       Texture3D
// ----------------------------------------------------------------------

// Texture3D implementation
Texture3D::Texture3D() {
  target = GL_TEXTURE_3D;
}

void Texture3D::Allocate(GLint w, GLint h, GLint d, GLenum internalFmt) {
  width          = w;
  height         = h;
  depth          = d;
  internalFormat = internalFmt;

  auto [sourceType, format] = GetTextureFormatInfo(internalFmt);

  Bind();
  glTexImage3D(
    target,
    0,
    internalFormat,
    width,
    height,
    depth,
    0,
    format,
    sourceType,
    nullptr
  );

  SetSamplerState(GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);
}

void Texture3D::SetSubImage(
  GLint       x,
  GLint       y,
  GLint       z,
  GLint       w,
  GLint       h,
  GLint       d,
  GLenum      format,
  GLenum      type,
  const void* data
) {
  Bind();
  glTexSubImage3D(target, 0, x, y, z, w, h, d, format, type, data);
}

// ----------------------------------------------------------------------
//                       TextureCube
// ----------------------------------------------------------------------

// TextureCube implementation
TextureCube::TextureCube(uint size, GLenum fmt) {
  target = GL_TEXTURE_CUBE_MAP;
  Allocate(size, fmt);
}

void TextureCube::Allocate(uint size, GLenum internalFmt) {
  width          = size;
  height         = size;
  depth          = 1;
  internalFormat = internalFmt;

  auto [sourceType, format] = GetTextureFormatInfo(internalFmt);

  Bind();

  GLenum targets[] = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
  };

  for (auto t : targets) {
    glTexImage2D(
      t,
      0,
      internalFormat,
      size,
      size,
      0,
      format,
      sourceType,
      nullptr
    );
  }

  SetSamplerState(GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);
}

void TextureCube::SetFaceSubImage(
  uint        face,
  GLint       x,
  GLint       y,
  GLint       w,
  GLint       h,
  GLenum      format,
  GLenum      type,
  const void* data
) {
  Bind();
  glTexSubImage2D(
    GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
    0,
    x,
    y,
    w,
    h,
    format,
    type,
    data
  );
}