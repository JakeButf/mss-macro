#include "main.h"
#include "utils/config.h"
#include "utils/macro.h"
#include <utils/logger.h>

#include <wups.h>

#include <vpad/input.h>

#include <malloc.h>

WUPS_PLUGIN_NAME("mss-macro");
WUPS_PLUGIN_DESCRIPTION("MSS macro plugin.");
WUPS_PLUGIN_VERSION(PLUGIN_VERSION_FULL);
WUPS_PLUGIN_AUTHOR("JakeZSR");
WUPS_PLUGIN_LICENSE("GPL");

WUPS_USE_WUT_DEVOPTAB();
WUPS_USE_STORAGE("mss-macro"); // Unique id for the storage api

void InitMacroConfigFromStorage() {
    WUPSStorageError storageError;
    auto subItemConfig = WUPSStorageAPI::GetOrCreateSubItem(CAT_CONFIG, storageError);
    if (!subItemConfig) {
        DEBUG_FUNCTION_LINE_ERR("Failed to get or create sub category \"%s\" for macro config", CAT_CONFIG);
        return;
    }
    
    if (subItemConfig->GetOrStoreDefault(MACRO_ENABLED_CONFIG_ID, gMacroEnabled, MACRO_ENABLED_DEFAULT) == WUPS_STORAGE_ERROR_SUCCESS) {
        SetMacroEnabled(gMacroEnabled);
    } else {
        DEBUG_FUNCTION_LINE_ERR("Failed to get or create item \"%s\"", MACRO_ENABLED_CONFIG_ID);
    }
    
    if (subItemConfig->GetOrStoreDefault(MACRO_DELAY_CONFIG_ID, gMacroDelay, MACRO_DELAY_DEFAULT) == WUPS_STORAGE_ERROR_SUCCESS) {
        SetMacroDelay(static_cast<uint32_t>(gMacroDelay));
    } else {
        DEBUG_FUNCTION_LINE_ERR("Failed to get or create item \"%s\"", MACRO_DELAY_CONFIG_ID);
    }
    
    if (WUPSStorageAPI::SaveStorage() != WUPS_STORAGE_ERROR_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("Failed to save storage");
    }
}


INITIALIZE_PLUGIN() {
    initLogging();

    InitMacroSystem();
    InitMacroConfigFromStorage();

    InitConfigMenu();
}

ON_APPLICATION_START() {
    initLogging();
}

ON_APPLICATION_ENDS() {
    deinitLogging();
}

DEINITIALIZE_PLUGIN() {
    deinitLogging();
}

DECL_FUNCTION(int32_t, VPADRead, VPADChan chan, VPADStatus *buffer, uint32_t buffer_size, VPADReadError *error) {
    VPADReadError real_error;
    int32_t result = real_VPADRead(chan, buffer, buffer_size, &real_error);
    
    if (result > 0 && real_error == VPAD_READ_SUCCESS && buffer != nullptr) {
        // Process macro input and get modified button state
        uint32_t modifiedButtons = ProcessMacroInput(buffer, buffer->hold);
        
        // Update the buffer with modified button state
        buffer->hold    = modifiedButtons;
        buffer->trigger = modifiedButtons & ~buffer->release;
        buffer->release = ~modifiedButtons & buffer->release;
    }
    
    if (error) {
        *error = real_error;
    }
    
    return result;
}

WUPS_MUST_REPLACE(VPADRead, WUPS_LOADER_LIBRARY_VPAD, VPADRead);
