#include "image.hpp"
#include <cstring>

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

Image2D::Image2D() : width(0), height(0), comps(0) {
}

Image2D::Image2D(uint w, uint h, uint c)
    : width(w), height(h), comps(c), pix(w * h * c, 0) {
}

Image2D::Image2D(uint w, uint h, uint c, const std::vector<uch>& pixelData)
    : width(w), height(h), comps(c), pix(pixelData) {
}

void Image2D::SetPixel(uint x, uint y, const std::vector<uch>& pixelData) {
  if (x >= width || y >= height || pixelData.size() != comps) return;

  size_t index = (y * width + x) * comps;
  for (size_t i = 0; i < comps; ++i) {
    pix[index + i] = pixelData[i];
  }
}

bool Image2D::WriteFile(const std::string& filepath, bool flip_y) const {
  if (pix.empty() || width == 0 || height == 0) return false;

  // Prepare flipped data buffer since stb_image_write expects top-to-bottom
  std::vector<unsigned char> out(pix.size());
  if (flip_y)
    FillFlipped(out.data(), pix.data(), width, height, comps);
  else
    std::memcpy(out.data(), pix.data(), width * height * comps);

  // fallback to PNG
  return stbi_write_png(
           filepath.c_str(),
           width,
           height,
           comps,
           out.data(),
           width * comps
         )
         != 0;
}

bool Image2D::FromFile(const std::string& filepath) {
  int            w, h, c;
  unsigned char* rawData = stbi_load(filepath.c_str(), &w, &h, &c, 0);
  if (!rawData) return false;

  width  = static_cast<uint>(w);
  height = static_cast<uint>(h);
  comps  = static_cast<uint>(c);

  pix.resize(width * height * comps);

  // Apply FillFlipped if the raw data needs to be stored y-mirrored in
  FillFlipped(pix.data(), rawData, width, height, comps);

  stbi_image_free(rawData);
  return true;
}

void Image2D::FillFlipped(
  uch*       dst,
  const uch* src,
  uint       width,
  uint       height,
  uint       comps
) {

  uint x, y, dst_index, src_index;

  for (size_t i = 0; i < width * height; ++i) {

    x = i % width;
    y = i / width;

    dst_index = i * comps;
    src_index = (x + (height - y - 1) * width) * comps;

    for (uint c = 0; c < comps; ++c) {
      dst[dst_index + c] = src[src_index + c];
    }
  }
}
