#pragma once

#include <cstdint>
#include <vpad/input.h>

//combo to start macro: L + R + DPad Down
#define MACRO_TRIGGER_COMBO (VPAD_BUTTON_ZR | VPAD_BUTTON_R | VPAD_BUTTON_X | VPAD_BUTTON_A)

#define MACRO_DELAY_NERF_DEFAULT_MS 100
#define MACRO_ENABLED_DEFAULT  false
#define MACRO_STICK_INPUTS_DEFAULT false

#define FRAME_TIME_MS 33
#define FRAMES_TO_MS(frames) ((frames) * FRAME_TIME_MS)

enum MacroStep {
    STEP_HOLD_DOWN = 0,
    STEP_PRESS_PLUS_1,
    STEP_WAIT_NERF,
    STEP_WAIT_3_FRAMES,
    STEP_HOLD_STICK_PLUS_2,
    STEP_WAIT_8_FRAMES,
    STEP_PRESS_PLUS_3,
};

struct MacroState {
    bool isActive;
    bool isEnabled;
    bool stickInputsEnabled;
    uint32_t delayNerfMs;
    uint64_t lastExecutionTime;
    uint64_t stepStartTime;
    bool triggerHeld;
    MacroStep currentStep;
    bool stickDirectionDown;
    uint32_t frameCounter;
};

void InitMacroSystem();

void ProcessMacroInput(VPADStatus *buffer);

MacroState* GetMacroState();

void SetMacroEnabled(bool enabled);
void SetMacroDelayNerf(uint32_t delayNerfMs);
void SetMacroStickInputs(bool enabled);

uint32_t GetMacroDelayNerf();
bool GetMacroStickInputs();
bool IsMacroActive();
