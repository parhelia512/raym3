#include "raym3/rendering/SvgModel.h"
#include <cstdio>

#define NANOSVG_IMPLEMENTATION
#include "external/nanosvg.h"

namespace raym3 {

SvgModel::SvgModel() : image(nullptr) {}

SvgModel::~SvgModel() { Unload(); }

bool SvgModel::LoadFromFile(const char *filename) {
  Unload();
  // Load SVG with 96 DPI
  image = nsvgParseFromFile(filename, "px", 96.0f);
  return image != nullptr;
}

bool SvgModel::LoadFromString(const char *data) {
  Unload();
  char *input = const_cast<char *>(
      data); // nsvgParse expects char*, but doesn't modify it usually
  image = nsvgParse(input, "px", 96.0f);
  return image != nullptr;
}

void SvgModel::Unload() {
  if (image) {
    nsvgDelete(image);
    image = nullptr;
  }
}

void SvgModel::SetColor(unsigned int color) {
  if (!image)
    return;

  for (NSVGshape *shape = image->shapes; shape != nullptr;
       shape = shape->next) {
    shape->fill.color = color;
    // Also set stroke if needed? usually icons are filled.
    if (shape->stroke.type != NSVG_PAINT_NONE) {
      shape->stroke.color = color;
    }
  }
}

} // namespace raym3
