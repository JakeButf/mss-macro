#pragma once

#include <cstdint>
#include <vpad/input.h>

//combo to start macro
#define MACRO_TRIGGER_COMBO (VPAD_BUTTON_L | VPAD_BUTTON_R | VPAD_BUTTON_DOWN)

#define MACRO_DELAY_DEFAULT_MS 100
#define MACRO_ENABLED_DEFAULT  false

struct MacroState {
    bool isActive;
    bool isEnabled;
    uint32_t delayMs;
    uint64_t lastExecutionTime;
    bool triggerHeld;
};

void InitMacroSystem();

//returns modified button state
uint32_t ProcessMacroInput(VPADStatus *buffer, uint32_t originalButtons);

MacroState* GetMacroState();

void SetMacroEnabled(bool enabled);
void SetMacroDelay(uint32_t delayMs);

uint32_t GetMacroDelay();

bool IsMacroActive();
