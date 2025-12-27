#pragma once

#include <string>

#include <cstdint>

#define CAT_CONFIG                             "config"

#define USTEALTH_CONFIG_ID                     "ustealth"
#define POWEROFFWARNING_CONFIG_ID              "SkipPowerOffWarning"
#define FORCE_NDM_SUSPEND_SUCCESS_CONFIG_ID    "forceNDMSuspendSuccess"
#define ALLOW_ERROR_NOTIFICATIONS              "allowErrorNotifications"

#define ACTIVATE_USTEALTH_DEFAULT              false
#define SKIP_4_SECOND_OFF_STATUS_CHECK_DEFAULT true
#define FORCE_NDM_SUSPEND_SUCCESS_DEFAULT      true
#define ALLOW_ERROR_NOTIFICATIONS_DEFAULT      true

extern bool gActivateUStealth;
extern bool gSkip4SecondOffStatusCheck;
extern bool gForceNDMSuspendSuccess;
extern bool gAllowErrorNotifications;

void InitConfigMenu();