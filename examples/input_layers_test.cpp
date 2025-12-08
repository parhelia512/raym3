#include "raym3/raym3.h"
#include <raylib.h>
#include <string>

int main() {
  InitWindow(800, 600, "raym3 Input Layers Test");
  SetTargetFPS(60);
  
  raym3::Initialize();
  raym3::SetTheme(false); // Light mode
  
  char textBuffer[256] = "Click me!";
  bool checked = false;
  float sliderValue = 50.0f;
  int clickCount = 0;
  
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(raym3::Theme::GetColorScheme().surface);
    
    raym3::BeginFrame();
    
    // Layer 0 - Background button
    if (raym3::Button("Background Button (Layer 0)", {100, 100, 200, 40})) {
      clickCount++;
    }
    
    // Layer 1 - Overlay panel with card
    raym3::PushLayer(1);
    
    // Card blocks input to background
    raym3::Card({200, 150, 400, 300}, raym3::CardVariant::Elevated);
    
    // Button on the card (should work)
    if (raym3::Button("Overlay Button", {220, 170, 120, 40})) {
      clickCount++;
    }
    
    // Text field on the card (should work)
    raym3::TextField(textBuffer, sizeof(textBuffer), {220, 230, 200, 56}, "Name");
    
    // Checkbox on the card (should work)
    raym3::Checkbox("Check me", {220, 300, 200, 24}, &checked);
    
    // Slider on the card (should work)
    sliderValue = raym3::Slider({220, 340, 200, 40}, sliderValue, 0.0f, 100.0f, "Slider");
    
    raym3::PopLayer();
    
    // Display click count
    raym3::Text(("Clicks: " + std::to_string(clickCount)).c_str(), 
                {100, 500, 200, 30}, 16, raym3::Theme::GetColorScheme().onSurface);
    
    raym3::EndFrame();
    EndDrawing();
  }
  
  raym3::Shutdown();
  CloseWindow();
  return 0;
}

