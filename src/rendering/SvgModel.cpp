#include "raym3/rendering/SvgModel.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

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
  // nsvgParse modifies the input string, so we must make a copy
  size_t len = strlen(data);
  char *input = (char *)malloc(len + 1);
  if (!input) return false;
  memcpy(input, data, len + 1);
  image = nsvgParse(input, "px", 96.0f);
  free(input);
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
