#include "raym3/layout/Layout.h"
#include <iostream>
#include <vector>

namespace raym3 {
    Rectangle GetTabContentScissorBounds() {
        return {0, 0, 1280, 800}; // Dummy screen bounds
    }
}

// Reproduction case for View3D layout issue
int main() {
    // Initialize Raylib (required for input functions used in Layout)
    SetTraceLogLevel(LOG_NONE); // Suppress raylib logs
    InitWindow(1280, 800, "Layout Repro");
    
    Rectangle screen = {0, 0, 1280, 800};
    
    // Run 2 frames to get calculated bounds
    for (int frame = 0; frame < 2; frame++) {
        raym3::Layout::Begin(screen);
        
        // Root
        raym3::LayoutStyle rootStyle = raym3::Layout::Column();
        rootStyle.flexGrow = 1;
        raym3::Layout::BeginContainer(rootStyle);
        
        // Toolbar
        raym3::Layout::Alloc(raym3::Layout::Fixed(-1, 32));
        // TabBar
        raym3::Layout::Alloc(raym3::Layout::Fixed(-1, 34));
        
        // TabContent
        raym3::LayoutStyle tabContentStyle = raym3::Layout::Column();
        tabContentStyle.flexGrow = 1;
        raym3::Layout::BeginContainer(tabContentStyle);
        
            // Middle Area
            raym3::LayoutStyle middleStyle = raym3::Layout::Row();
            middleStyle.height = 0; // Force flex basis 0
            middleStyle.flexGrow = 1;
            middleStyle.align = 4; // Stretch
            raym3::Layout::BeginContainer(middleStyle);
            
                // Hierarchy (Column, Fixed Width)
                raym3::LayoutStyle hierStyle = raym3::Layout::Column();
                hierStyle.width = 300;
                hierStyle.flexGrow = 0;
                hierStyle.align = 4;
                Rectangle hierBounds = raym3::Layout::BeginContainer(hierStyle);
                    // Hierarchy logic often puts a ScrollContainer inside
                    raym3::LayoutStyle hierScrollStyle = raym3::Layout::Column();
                    hierScrollStyle.flexGrow = 1;
                    raym3::Layout::BeginScrollContainer(hierScrollStyle, false, true);
                    raym3::Layout::EndContainer(); // End Scroll
                raym3::Layout::EndContainer(); // End Hierarchy
                
                // Divider 1
                raym3::Layout::Alloc(raym3::Layout::Fixed(2, -1));
                
                // Viewport Container
                raym3::LayoutStyle vpStyle = raym3::Layout::Column();
                vpStyle.flexGrow = 1;
                raym3::Layout::BeginContainer(vpStyle);
                    // Spacer
                    raym3::Layout::Alloc(raym3::Layout::Fixed(-1, 20)); // vTopMargin
                    
                    // View3D Alloc
                    Rectangle viewBounds = raym3::Layout::Alloc(raym3::Layout::Flex(1));
                    
                raym3::Layout::EndContainer();
                
                // Divider 2
                raym3::Layout::Alloc(raym3::Layout::Fixed(2, -1));
                
                // Inspector
                raym3::LayoutStyle inspStyle = raym3::Layout::Column();
                inspStyle.width = 300;
                inspStyle.flexGrow = 0;
                inspStyle.align = 4;
                Rectangle inspBounds = raym3::Layout::BeginContainer(inspStyle);
                    // Inspector ScrollContainer
                    raym3::LayoutStyle inspScrollStyle = raym3::Layout::Column();
                    inspScrollStyle.flexGrow = 1;
                    raym3::Layout::BeginScrollContainer(inspScrollStyle, false, true);
                    raym3::Layout::EndContainer(); // End Scroll
                raym3::Layout::EndContainer(); // End Inspector
                
            raym3::Layout::EndContainer(); // Middle
            
        raym3::Layout::EndContainer(); // TabContent
        raym3::Layout::EndContainer(); // Root
        
        raym3::Layout::End();
        
        if (frame == 1) {
            std::cout << "Hierarchy: " << hierBounds.x << ", " << hierBounds.y 
                      << " " << hierBounds.width << "x" << hierBounds.height << std::endl;
            std::cout << "Viewport: " << viewBounds.x << ", " << viewBounds.y 
                      << " " << viewBounds.width << "x" << viewBounds.height << std::endl;
            std::cout << "Inspector: " << inspBounds.x << ", " << inspBounds.y 
                      << " " << inspBounds.width << "x" << inspBounds.height << std::endl;
                      
            // Check intersection
            if (viewBounds.x < hierBounds.x + hierBounds.width) {
                std::cout << "OVERLAP DETECTED: Viewport overlaps Hierarchy!" << std::endl;
            }
        }
    }
    CloseWindow();
    return 0;
}
