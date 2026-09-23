#pragma once

#include "core/utils.hpp"
#include <string>
#include <vector>

class Image2D {
private:

  uint             width;
  uint             height;
  uint             comps;
  std::vector<uch> pix;

public:

  Image2D();
  Image2D(uint width, uint height, uint comps);
  Image2D(uint width, uint height, uint comps, const std::vector<uch>& data);

  // Copy and move

  Image2D(Image2D&&) = default;
  Image2D& operator=(Image2D&&) = default;

  Image2D(const Image2D&) = default;
  Image2D& operator=(const Image2D&) = default;

  // Functions

  void SetPixel(uint x, uint y, const std::vector<uch>& pixelData);
  bool WriteFile(const std::string& filepath, bool flip_y = true) const;
  bool FromFile(const std::string& filepath);

  static void
    FillFlipped(uch* dst, const uch* src, uint width, uint height, uint comps);

  // Getters
  uint Width() const {
    return width;
  }

  uint Height() const {
    return height;
  }

  uint Comps() const {
    return comps;
  }

  const uch* Pix() const {
    return pix.data();
  }
};