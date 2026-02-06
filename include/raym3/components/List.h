#pragma once

#include <raylib.h>
#include <functional>
#include "raym3/types.h"

namespace raym3 {

struct ListItem {
  const char *text = nullptr;
  const char *leadingIcon = nullptr;
  const char *secondaryActionIcon = nullptr;
  ListItem *children = nullptr;
  int childCount = 0;
  bool expanded = false;
  bool selected = false;
  bool disabled = false;
  bool enableDrag = false;
  Color textColor = BLANK;
  Color iconColor = BLANK;
  Color backgroundColor = BLANK;
  void* userData = nullptr;
  const char *tooltip = nullptr;
  TooltipPlacement tooltipPlacement = TooltipPlacement::Auto;
};

typedef std::function<void(ListItem*, int)> ListSelectionCallback;
typedef std::function<void(int fromIndex, int toIndex)> ListDragCallback;

void List(Rectangle bounds, ListItem *items, int itemCount,
          float *outHeight = nullptr, 
          ListSelectionCallback onSelectionChange = nullptr,
          ListDragCallback onDragReorder = nullptr);

// Get current drag state (for external rendering)
bool ListIsDragging();
int ListGetDragSourceIndex();
int ListGetDragTargetIndex();

} // namespace raym3

