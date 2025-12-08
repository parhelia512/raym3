#pragma once

#include <raylib.h>
#include <functional>

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
  Color textColor = BLANK;
  Color iconColor = BLANK;
  Color backgroundColor = BLANK;
};

typedef std::function<void(ListItem*, int)> ListSelectionCallback;

void List(Rectangle bounds, ListItem *items, int itemCount,
          float *outHeight = nullptr, ListSelectionCallback onSelectionChange = nullptr);

} // namespace raym3
