#pragma once

#include <string>

#include <cstdint>

#define CAT_CONFIG                             "config"

#define MACRO_ENABLED_CONFIG_ID                "macroEnabled"
#define MACRO_DELAY_CONFIG_ID                  "macroDelay"

#define MACRO_ENABLED_DEFAULT                  false
#define MACRO_DELAY_DEFAULT                    100

extern bool gMacroEnabled;
extern int32_t gMacroDelay;

void InitConfigMenu();