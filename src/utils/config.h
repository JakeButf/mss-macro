#pragma once

#include <string>

#include <cstdint>

#define CAT_CONFIG "config"

#define MACRO_ENABLED_CONFIG_ID "macroEnabled"
#define MACRO_DELAY_NERF_CONFIG_ID "macroDelayNerf"
#define MACRO_STICK_INPUTS_CONFIG_ID "macroStickInputs"

#define MACRO_ENABLED_DEFAULT false
#define MACRO_DELAY_NERF_DEFAULT 100
#define MACRO_STICK_INPUTS_DEFAULT true

extern bool gMacroEnabled;
extern int32_t gMacroDelayNerf;
extern bool gMacroStickInputs;

void InitConfigMenu();