#pragma once

#include "image.hpp"
#include "resource.hpp"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

// Helper function to determine OpenGL source type and format from internal format
struct TextureFormatInfo {
  GLenum SourceType;  // GL_UNSIGNED_BYTE, GL_FLOAT, etc.
  GLenum Format;      // GL_RGBA, GL_RGB, GL_DEPTH_COMPONENT, etc.
};

TextureFormatInfo GetTextureFormatInfo(GLenum internalFormat);

class Texture : public Resource {
protected:
  GLuint target;
  GLint  width;
  GLint  height;
  GLint  depth;
  GLenum internalFormat;

public:
  Texture();
  virtual ~Texture();

  // Delete copy
  Texture(const Texture&)             = delete;
  Texture& operator =(const Texture&) = delete;

  // Move operations
  Texture(Texture&& other) noexcept;
  Texture& operator =(Texture&& other) noexcept;

  // Common getters
  GLuint Target() const;
  GLint Width() const;
  GLint Height() const;
  GLint Depth() const;
  GLenum InternalFormat() const;

  // Binding
  void Bind(GLuint unit = 0) const;

  // Generate mipmaps
  void GenerateMipmaps() const;

  // Set sampler state
  void SetSamplerState(
    GLenum wrapMode  = GL_REPEAT,
    GLenum magFilter = GL_NEAREST,
    GLenum minFilter = GL_NEAREST_MIPMAP_LINEAR
  ) const;

  // Set individual parameter
  void SetParameter(GLenum pname, GLint param) const;
  void SetParameterf(GLenum pname, GLfloat param) const;
};

class Texture2D : public Texture {
public:
  Texture2D();

  // Create from Image2D
  void FromImage(const Image2D& image, GLenum internalFmt = GL_RGBA8);

  // Create empty texture
  void Allocate(GLint w, GLint h, GLenum internalFmt = GL_RGBA8);

  // Create from raw data pointer
  void FromData(const uch* data, GLint w, GLint h, GLenum internalFmt = GL_RGBA8);

  // Update sub-region
  void SetSubImage(
    GLint       x,
    GLint       y,
    GLint       w,
    GLint       h,
    GLenum      format,
    GLenum      type,
    const void* data
  );
};

class Texture3D : public Texture {
public:
  Texture3D();

  // Create empty 3D texture
  void Allocate(GLint w, GLint h, GLint d, GLenum internalFmt = GL_RGBA8);

  // Update sub-region
  void SetSubImage(
    GLint       x,
    GLint       y,
    GLint       z,
    GLint       w,
    GLint       h,
    GLint       d,
    GLenum      format,
    GLenum      type,
    const void* data
  );
};

class TextureCube : public Texture {
public:
  TextureCube(uint size, GLenum fmt = GL_RGBA8);

  // Create empty cube map
  void Allocate(uint size, GLenum internalFmt = GL_RGBA8);

  // Update single face
  void SetFaceSubImage(
    uint        face,
    GLint       x,
    GLint       y,
    GLint       w,
    GLint       h,
    GLenum      format,
    GLenum      type,
    const void* data
  );
};