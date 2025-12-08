#include "raym3/components/Icon.h"
#include "raym3/rendering/SvgRenderer.h"

namespace raym3 {

void IconComponent::Render(const char *name, Rectangle bounds,
                           IconVariation variation, Color color) {
  SvgRenderer::DrawIcon(name, bounds, variation, color);
}

} // namespace raym3
