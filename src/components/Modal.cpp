#include "raym3/components/Modal.h"
#include "raym3/components/Button.h"
#include "raym3/components/Card.h"
#include "raym3/components/Text.h"
#include "raym3/components/TextField.h"
#include "raym3/rendering/Renderer.h"
#include "raym3/styles/Theme.h"
#include <cstring>
#include <raylib.h>

#if RAYM3_USE_INPUT_LAYERS
#include "raym3/input/InputLayer.h"
#endif

namespace raym3 {

bool ModalComponent::isOpen_ = false;
bool ModalComponent::firstFrame_ = false;

bool ModalComponent::Render(const char* title, const char* message,
                            const char* textFieldLabel, char* textBuffer, size_t bufferSize,
                            const char* confirmButton, const char* cancelButton) {
    if (!isOpen_) {
        isOpen_ = true;
        firstFrame_ = true;
    }

#if RAYM3_USE_INPUT_LAYERS
    InputLayerManager::PushLayer(9999);
    Rectangle screenBounds = {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()};
    InputLayerManager::RegisterBlockingRegion(screenBounds, true);
#endif

    DrawBackdrop();

    Rectangle modalBounds = GetModalBounds();
    ColorScheme &scheme = Theme::GetColorScheme();
    float cornerRadius = Theme::GetShapeTokens().cornerExtraLarge;

    // Draw modal card
    Renderer::DrawElevatedRectangle(modalBounds, cornerRadius, 3, scheme.surface);

    float padding = 24.0f;
    float y = modalBounds.y + padding;
    float contentWidth = modalBounds.width - padding * 2;

    // Title
    if (title) {
        Vector2 titlePos = {modalBounds.x + padding, y};
        Renderer::DrawText(title, titlePos, 24.0f, scheme.onSurface, FontWeight::Regular);
        y += 40.0f;
    }

    // Message
    if (message) {
        Vector2 messagePos = {modalBounds.x + padding, y};
        Renderer::DrawText(message, messagePos, 14.0f, scheme.onSurfaceVariant, FontWeight::Regular);
        y += 30.0f;
    }

    // Text Field
    if (textFieldLabel && textBuffer) {
        Rectangle fieldBounds = {modalBounds.x + padding, y, contentWidth, 56.0f};
        
        TextFieldOptions options;
        options.placeholder = textFieldLabel;
        options.variant = TextFieldVariant::Filled;
        
        TextFieldComponent::Render(textBuffer, bufferSize, fieldBounds, nullptr, options);
        y += 72.0f; // Field height + spacing
    }

    firstFrame_ = false;

    // Buttons
    float buttonHeight = 40.0f;
    float buttonY = modalBounds.y + modalBounds.height - padding - buttonHeight;
    float currentX = modalBounds.x + modalBounds.width - padding;

    bool result = false;
    bool shouldClose = false;

    // Confirm button (right)
    if (confirmButton) {
        Vector2 textSize = Renderer::MeasureText(confirmButton, 14.0f, FontWeight::Medium);
        float btnWidth = textSize.x + 24.0f;
        if (btnWidth < 60.0f) btnWidth = 60.0f;
        
        currentX -= btnWidth;
        Rectangle confirmBounds = {currentX, buttonY, btnWidth, buttonHeight};
        
        if (ButtonComponent::Render(confirmButton, confirmBounds, ButtonVariant::Filled)) {
            result = true;
            shouldClose = true;
        }
        
        currentX -= 8.0f;
    }

    // Cancel button (left of confirm)
    if (cancelButton) {
        Vector2 textSize = Renderer::MeasureText(cancelButton, 14.0f, FontWeight::Medium);
        float btnWidth = textSize.x + 24.0f;
        if (btnWidth < 60.0f) btnWidth = 60.0f;
        
        currentX -= btnWidth;
        Rectangle cancelBounds = {currentX, buttonY, btnWidth, buttonHeight};
        
        if (ButtonComponent::Render(cancelButton, cancelBounds, ButtonVariant::Text)) {
            result = false;
            shouldClose = true;
        }
    }

    // Handle Escape key to close
    if (IsKeyPressed(KEY_ESCAPE)) {
        result = false;
        shouldClose = true;
    }

    // Handle Enter key to confirm
    if (IsKeyPressed(KEY_ENTER) && textBuffer && strlen(textBuffer) > 0) {
        result = true;
        shouldClose = true;
    }

#if RAYM3_USE_INPUT_LAYERS
    InputLayerManager::PopLayer();
#endif

    if (shouldClose) {
        isOpen_ = false;
        return result;
    }

    return false;
}

void ModalComponent::DrawBackdrop() {
    Rectangle backdrop = {0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()};
    ColorScheme &scheme = Theme::GetColorScheme();
    Color scrimColor = ColorAlpha(scheme.scrim, 0.32f);
    DrawRectangleRec(backdrop, scrimColor);
}

Rectangle ModalComponent::GetModalBounds() {
    float width = 400.0f;
    float height = 300.0f;
    
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    return {
        (screenWidth - width) / 2.0f,
        (screenHeight - height) / 2.0f,
        width,
        height
    };
}

} // namespace raym3

