#pragma once

#include "resource.hpp"
#include <vector>

class Framebuffer : public Resource {
public:

  // Конструктор и деструктор
  Framebuffer();
  ~Framebuffer();

  // Копирование запрещено
  Framebuffer(const Framebuffer& other) = delete;
  Framebuffer& operator =(const Framebuffer& other) = delete;

  // Семантика перемещения
  Framebuffer(Framebuffer&& other) noexcept;
  Framebuffer& operator =(Framebuffer&& other) noexcept;

  // Управление привязкой
  void        Bind(GLenum target = GL_FRAMEBUFFER) const;
  static void Unbind(GLenum target = GL_FRAMEBUFFER);

  // Подключение внешних ресурсов
  void AttachTexture(GLenum attachment, GLuint textureId, GLint level = 0);
  void AttachRenderbuffer(GLenum attachment, GLuint rboId);

  // Настройка вывода (Draw Buffers)
  void SetDrawBuffers(const std::vector<GLenum>& buffers);

  // Проверка статуса готовности
  bool IsComplete() const;

};
