#include "macro.h"
#include "logger.h"

#include <coreinit/time.h>
#include <vpad/input.h>

static MacroState sMacroState = {
    .isActive = false,
    .isEnabled = MACRO_ENABLED_DEFAULT,
    .delayMs = MACRO_DELAY_DEFAULT_MS,
    .lastExecutionTime = 0,
    .triggerHeld = false
};

static inline uint64_t MsToTicks(uint32_t ms) {
    return OSMillisecondsToTicks(ms);
}

void InitMacroSystem() {
    DEBUG_FUNCTION_LINE_INFO("Initializing macro system...");
    sMacroState.isActive          = false;
    sMacroState.isEnabled         = MACRO_ENABLED_DEFAULT;
    sMacroState.delayMs           = MACRO_DELAY_DEFAULT_MS;
    sMacroState.lastExecutionTime = 0;
    sMacroState.triggerHeld       = false;
    DEBUG_FUNCTION_LINE_INFO("Macro system initialized. Delay: %d ms, Enabled: %d", sMacroState.delayMs, sMacroState.isEnabled);
}

uint32_t ProcessMacroInput(VPADStatus *buffer, uint32_t originalButtons) {
    //if no macro just take input
    if (!sMacroState.isEnabled) {
        return originalButtons;
    }

    bool triggerPressed = (originalButtons & MACRO_TRIGGER_COMBO) == MACRO_TRIGGER_COMBO;
    
    if (triggerPressed && !sMacroState.triggerHeld) {
        sMacroState.triggerHeld = true;
        sMacroState.isActive = !sMacroState.isActive;
        
        if (sMacroState.isActive) {
            DEBUG_FUNCTION_LINE_INFO("Macro activated!");
            sMacroState.lastExecutionTime = OSGetTime();
        } else {
            DEBUG_FUNCTION_LINE_INFO("Macro deactivated!");
        }
        
        return originalButtons & ~MACRO_TRIGGER_COMBO;
    } else if (!triggerPressed) {
        sMacroState.triggerHeld = false;
    }
    
    if (!sMacroState.isActive) {
        return originalButtons;
    }
    
    uint64_t currentTime = OSGetTime();
    uint64_t timeSinceLastExecution = currentTime - sMacroState.lastExecutionTime;
    uint64_t delayTicks = MsToTicks(sMacroState.delayMs);
    
    if (timeSinceLastExecution >= delayTicks) {
        sMacroState.lastExecutionTime = currentTime;
        
        uint32_t modifiedButtons = originalButtons | VPAD_BUTTON_A;
        
        DEBUG_FUNCTION_LINE_VERBOSE("Macro: Injecting A button press (delay: %d ms)", sMacroState.delayMs);
        
        return modifiedButtons;
    }
    
    return originalButtons;
}

MacroState* GetMacroState() {
    return &sMacroState;
}

void SetMacroEnabled(bool enabled) {
    bool wasActive = sMacroState.isActive;
    sMacroState.isEnabled = enabled;
    
    if (!enabled && wasActive) {
        sMacroState.isActive = false;
        DEBUG_FUNCTION_LINE_INFO("Macro system disabled - deactivating active macro");
    }
    
    DEBUG_FUNCTION_LINE_INFO("Macro system %s", enabled ? "enabled" : "disabled");
}

void SetMacroDelay(uint32_t delayMs) {
    if (delayMs < 10) {
        delayMs = 10;
    } else if (delayMs > 10000) {
        delayMs = 10000;
    }
    
    sMacroState.delayMs = delayMs;
    DEBUG_FUNCTION_LINE_INFO("Macro delay set to %d ms", delayMs);
}

uint32_t GetMacroDelay() {
    return sMacroState.delayMs;
}

bool IsMacroActive() {
    return sMacroState.isActive;
}
