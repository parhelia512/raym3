#pragma once

#include <functional>
#include <raylib.h>
#include <string>
#include <vector>
#include "raym3/types.h"

namespace raym3 {

enum class TabState { Default, Hover, Active, Loading };

struct TabItem {
  std::string id;
  std::string title;
  const char *iconName = nullptr;
  bool isLoading = false;
  bool isAudioPlaying = false;
  bool closeable = true;
  const char *tooltip = nullptr;
  TooltipPlacement tooltipPlacement = TooltipPlacement::Auto;
};

struct TabBarOptions {
  // Colors - if BLANK, uses theme defaults
  Color activeTabColor = BLANK;      // Surface color for active tab
  Color inactiveTabColor = BLANK;    // Background for inactive tabs
  Color activeTextColor = BLANK;     // Text on active tab
  Color inactiveTextColor = BLANK;   // Text on inactive tabs
  Color dividerColor = BLANK;        // Divider between inactive tabs

  // Dimensions
  float tabHeight = 34.0f;           // Default 34px
  float minTabWidth = 90.0f;         // Minimum before collapse
  float maxTabWidth = 240.0f;        // Maximum width
  float cornerRadius = 10.0f;        // Top corner radius

  // Behavior
  bool showCloseOnHover = true;      // Close button only on hover
  bool showDividers = true;          // Dividers between tabs

  // Optional "Add Tab" button
  std::function<void()> onAddTab = nullptr;
  const char* addTabIcon = "add";    // Default icon
  
  // Instance ID for independent hover tracking (use different IDs for separate TabBars)
  int instanceId = 0;
};

class TabBarComponent {
public:
  // Render tab bar and return index of clicked tab (-1 if none)
  // closedTabIndex is set to index of closed tab (-1 if none clicked)
  static int Render(Rectangle bounds, const std::vector<TabItem> &items,
                    int selectedIndex, const TabBarOptions &options = {},
                    int *closedTabIndex = nullptr);

private:
  static void RenderTab(Rectangle bounds, const TabItem &item, bool isActive,
                        bool isHovered, bool showLeftDivider,
                        bool showRightDivider, const TabBarOptions &options);
  static bool RenderCloseButton(Rectangle tabBounds, bool isActive,
                                bool isHovered, const TabBarOptions &options);
  static std::string TruncateText(const std::string &text, float maxWidth,
                                  float fontSize);
};

//-----------------------------------------------------------------------------
// Tab Content Container
//-----------------------------------------------------------------------------

// Begin a tab content container (matches active tab background)
void TabContentBegin(Rectangle bounds, Color backgroundColor, bool clip = true);

// End tab content container
void TabContentEnd();

// Get current TabContent scissor bounds (returns screen bounds if not clipping)
Rectangle GetTabContentScissorBounds();

//-----------------------------------------------------------------------------
// Convenience API
//-----------------------------------------------------------------------------

// Render tab bar and return clicked tab index (-1 if none)
int TabBar(Rectangle bounds, const std::vector<TabItem> &items,
           int selectedIndex, const TabBarOptions &options = {},
           int *closedTabIndex = nullptr);

} // namespace raym3
