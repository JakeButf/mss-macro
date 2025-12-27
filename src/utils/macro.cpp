#include "macro.h"
#include "logger.h"
#include "watermark.h"

#include <coreinit/time.h>
#include <vpad/input.h>
#include <cstring>

static MacroState sMacroState = {
    .isActive = false,
    .isEnabled = MACRO_ENABLED_DEFAULT,
    .stickInputsEnabled = MACRO_STICK_INPUTS_DEFAULT,
    .delayNerfMs = MACRO_DELAY_NERF_DEFAULT_MS,
    .lastExecutionTime = 0,
    .stepStartTime = 0,
    .triggerHeld = false,
    .currentStep = STEP_HOLD_DOWN,
    .stickDirectionDown = true,
    .frameCounter = 0
};

static inline uint64_t MsToTicks(uint32_t ms) {
    return OSMillisecondsToTicks(ms);
}

static inline uint32_t TicksToMs(uint64_t ticks) {
    return OSTicksToMilliseconds(ticks);
}

void InitMacroSystem() {
    sMacroState.isActive = false;
    sMacroState.isEnabled = MACRO_ENABLED_DEFAULT;
    sMacroState.stickInputsEnabled = MACRO_STICK_INPUTS_DEFAULT;
    sMacroState.delayNerfMs = MACRO_DELAY_NERF_DEFAULT_MS;
    sMacroState.lastExecutionTime  = 0;
    sMacroState.stepStartTime = 0;
    sMacroState.triggerHeld = false;
    sMacroState.currentStep = STEP_HOLD_DOWN;
    sMacroState.stickDirectionDown = true;
    sMacroState.frameCounter = 0;
}

void ProcessMacroInput(VPADStatus *buffer) {
    if (!buffer) return;
    uint32_t originalButtons = buffer->hold;
    //if macro system is disabled just pass through original input
    if (!sMacroState.isEnabled) {
        return;
    }

    bool triggerPressed = (originalButtons & MACRO_TRIGGER_COMBO) == MACRO_TRIGGER_COMBO;
    
    if (triggerPressed && !sMacroState.triggerHeld) {
        sMacroState.triggerHeld = true;
        sMacroState.isActive = !sMacroState.isActive;
        
        if (sMacroState.isActive) {
            DEBUG_FUNCTION_LINE_INFO("Macro activated!");
            sMacroState.currentStep = STEP_HOLD_DOWN;
            sMacroState.stickDirectionDown = true;
            sMacroState.stepStartTime = OSGetTime();
            sMacroState.frameCounter = 0;
            ShowWatermark();
        } else {
            DEBUG_FUNCTION_LINE_INFO("Macro deactivated!");
            HideWatermark();
        }
        
        buffer->hold &= ~MACRO_TRIGGER_COMBO;
        buffer->trigger &= ~MACRO_TRIGGER_COMBO;
        return;
    } else if (!triggerPressed) {
        sMacroState.triggerHeld = false;
    }
    
    if (!sMacroState.isActive) {
        return;
    }
    
    uint64_t currentTime = OSGetTime();
    uint64_t stepElapsedTime = currentTime - sMacroState.stepStartTime;
    uint32_t stepElapsedMs = TicksToMs(stepElapsedTime);
    
    switch (sMacroState.currentStep) {
        case STEP_HOLD_DOWN:
            if (sMacroState.stickInputsEnabled) {
                buffer->leftStick.x = 0.0f;
                buffer->leftStick.y = -1.0f; // Down
            }
            sMacroState.currentStep = STEP_PRESS_PLUS_1;
            sMacroState.stepStartTime = currentTime;
            
        case STEP_PRESS_PLUS_1:
            buffer->hold = VPAD_BUTTON_PLUS;
            buffer->trigger = VPAD_BUTTON_PLUS;
            if (sMacroState.stickInputsEnabled && sMacroState.currentStep == STEP_PRESS_PLUS_1) {
                buffer->leftStick.x = 0.0f;
                buffer->leftStick.y = -1.0f;
            }
            
            if (stepElapsedMs >= FRAME_TIME_MS) {
                sMacroState.currentStep = STEP_WAIT_NERF;
                sMacroState.stepStartTime = currentTime;
            }
            break;
            
        case STEP_WAIT_NERF:
            if (sMacroState.stickInputsEnabled) {
                buffer->leftStick.x = 0.0f;
                buffer->leftStick.y = -1.0f;
            }
            
            if (stepElapsedMs >= sMacroState.delayNerfMs) {
                sMacroState.currentStep = STEP_WAIT_3_FRAMES;
                sMacroState.stepStartTime = currentTime;
                sMacroState.frameCounter = 0;
            }
            break;
            
        case STEP_WAIT_3_FRAMES:
            sMacroState.frameCounter = stepElapsedMs / FRAME_TIME_MS;
            
            if (sMacroState.stickInputsEnabled) {
                buffer->leftStick.x = 0.0f;
                buffer->leftStick.y = -1.0f;
            }
            
            if (sMacroState.frameCounter >= 1) {
                sMacroState.currentStep = STEP_HOLD_STICK_PLUS_2;
                sMacroState.stepStartTime = currentTime;
            }
            break;
            
        case STEP_HOLD_STICK_PLUS_2:
            if (sMacroState.stickInputsEnabled) {
                buffer->leftStick.x = 0.0f;
                buffer->leftStick.y = sMacroState.stickDirectionDown ? -1.0f : 1.0f;
            }
            buffer->hold = VPAD_BUTTON_PLUS;
            buffer->trigger = VPAD_BUTTON_PLUS;
            
            if (stepElapsedMs >= FRAME_TIME_MS) {
                sMacroState.currentStep = STEP_WAIT_8_FRAMES;
                sMacroState.stepStartTime = currentTime;
                sMacroState.frameCounter = 0;
            }
            break;
            
        case STEP_WAIT_8_FRAMES:
            sMacroState.frameCounter = stepElapsedMs / FRAME_TIME_MS;
            
            if (sMacroState.stickInputsEnabled) {
                buffer->leftStick.x = 0.0f;
                buffer->leftStick.y = sMacroState.stickDirectionDown ? -1.0f : 1.0f;
            }
            
            if (sMacroState.frameCounter >= 6) {
                sMacroState.currentStep = STEP_PRESS_PLUS_3;
                sMacroState.stepStartTime = currentTime;
            }
            break;
            
        case STEP_PRESS_PLUS_3:
            buffer->hold = VPAD_BUTTON_PLUS;
            buffer->trigger = VPAD_BUTTON_PLUS;
            
            if (sMacroState.stickInputsEnabled) {
                buffer->leftStick.x = 0.0f;
                buffer->leftStick.y = sMacroState.stickDirectionDown ? -1.0f : 1.0f;
            }
            
            if (stepElapsedMs >= FRAME_TIME_MS) {
                sMacroState.stickDirectionDown = !sMacroState.stickDirectionDown;
                sMacroState.currentStep = STEP_HOLD_DOWN;
                sMacroState.stepStartTime = currentTime;
            }
            break;
    }
}

MacroState* GetMacroState() {
    return &sMacroState;
}

void SetMacroEnabled(bool enabled) {
    bool wasActive = sMacroState.isActive;
    sMacroState.isEnabled = enabled;
    
    if (!enabled && wasActive) {
        sMacroState.isActive = false;
        HideWatermark();
    }
}

void SetMacroDelayNerf(uint32_t delayNerfMs) {
    if (delayNerfMs > 30000) {
        delayNerfMs = 30000;
    }
    
    sMacroState.delayNerfMs = delayNerfMs;
    
    if (sMacroState.isActive) {
        UpdateWatermark();
    }
}

void SetMacroStickInputs(bool enabled) {
    sMacroState.stickInputsEnabled = enabled;
    
    if (sMacroState.isActive) {
        UpdateWatermark();
    }
}

uint32_t GetMacroDelayNerf() {
    return sMacroState.delayNerfMs;
}

bool GetMacroStickInputs() {
    return sMacroState.stickInputsEnabled;
}

bool IsMacroActive() {
    return sMacroState.isActive;
}
