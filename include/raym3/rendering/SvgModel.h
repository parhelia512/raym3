#pragma once

#include <string>

// Forward declaration
struct NSVGimage;

namespace raym3 {

class SvgModel {
public:
  SvgModel();
  ~SvgModel();

  bool LoadFromFile(const char *filename);
  bool LoadFromString(const char *data);
  void Unload();

  NSVGimage *GetImage() const { return image; }

  // Helper to change fill color of all shapes
  void SetColor(unsigned int color); // 0xAABBGGRR (nanosvg format)

private:
  NSVGimage *image;
};

} // namespace raym3
