#include "config.h"

#include "logger.h"
#include "macro.h"

#include <wups/config/WUPSConfigCategory.h>
#include <wups/config/WUPSConfigItemBoolean.h>
#include <wups/config/WUPSConfigItemIntegerRange.h>
#include <wups/storage.h>

bool gMacroEnabled = MACRO_ENABLED_DEFAULT;
int32_t gMacroDelay = MACRO_DELAY_DEFAULT;

void boolItemChangedConfig(ConfigItemBoolean *item, bool newValue) {
    WUPSStorageError storageError;
    auto subItemConfig = WUPSStorageAPI::GetSubItem(CAT_CONFIG, storageError);
    if (!subItemConfig) {
        DEBUG_FUNCTION_LINE_ERR("Failed to get sub item \"%s\": %s", CAT_CONFIG, WUPSStorageAPI::GetStatusStr(storageError).data());
        return;
    }
    if (std::string_view(MACRO_ENABLED_CONFIG_ID) == item->identifier) {
        gMacroEnabled = newValue;
        SetMacroEnabled(newValue);
        storageError = subItemConfig->Store(MACRO_ENABLED_CONFIG_ID, newValue);
    } else {
        return;
    }
    if (storageError != WUPS_STORAGE_ERROR_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("Failed to store %s. New value was %d", item->identifier, newValue);
    }
}

void intItemChangedConfig(ConfigItemIntegerRange *item, int32_t newValue) {
    WUPSStorageError storageError;
    auto subItemConfig = WUPSStorageAPI::GetSubItem(CAT_CONFIG, storageError);
    if (!subItemConfig) {
        DEBUG_FUNCTION_LINE_ERR("Failed to get sub item \"%s\": %s", CAT_CONFIG, WUPSStorageAPI::GetStatusStr(storageError).data());
        return;
    }
    if (std::string_view(MACRO_DELAY_CONFIG_ID) == item->identifier) {
        gMacroDelay = newValue;
        SetMacroDelay(static_cast<uint32_t>(newValue));
        storageError = subItemConfig->Store(MACRO_DELAY_CONFIG_ID, newValue);
    } else {
        return;
    }
    if (storageError != WUPS_STORAGE_ERROR_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("Failed to store %s. New value was %d", item->identifier, newValue);
    }
}

WUPSConfigAPICallbackStatus ConfigMenuOpenedCallback(WUPSConfigCategoryHandle rootHandle) {
    try {
        WUPSConfigCategory root = WUPSConfigCategory(rootHandle);

        auto macroSettings = WUPSConfigCategory::Create("Macro Settings");

        macroSettings.add(WUPSConfigItemBoolean::Create(MACRO_ENABLED_CONFIG_ID,
                                                        "Enable macro system",
                                                        MACRO_ENABLED_DEFAULT, gMacroEnabled,
                                                        &boolItemChangedConfig));

        macroSettings.add(WUPSConfigItemIntegerRange::Create(MACRO_DELAY_CONFIG_ID,
                                                             "Macro delay (ms)",
                                                             MACRO_DELAY_DEFAULT, gMacroDelay,
                                                             10, 10000,
                                                             &intItemChangedConfig));

        root.add(std::move(macroSettings));

    } catch (std::exception &e) {
        DEBUG_FUNCTION_LINE_ERR("Exception: %s\n", e.what());
        return WUPSCONFIG_API_CALLBACK_RESULT_ERROR;
    }
    return WUPSCONFIG_API_CALLBACK_RESULT_SUCCESS;
}

void ConfigMenuClosedCallback() {
    WUPSStorageError storageError;
    if ((storageError = WUPSStorageAPI::SaveStorage()) != WUPS_STORAGE_ERROR_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("Failed to save storage: %d %s", storageError, WUPSStorageAPI_GetStatusStr(storageError));
    }
}

void InitConfigMenu() {
    WUPSConfigAPIOptionsV1 configOptions = {.name = "MSS Macro"};
    if (WUPSConfigAPI_Init(configOptions, ConfigMenuOpenedCallback, ConfigMenuClosedCallback) != WUPSCONFIG_API_RESULT_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("Failed to init config api");
    }
}