#include "main.h"
#include "utils/config.h"
#include "utils/utils.h"
#include <utils/logger.h>

#include <mocha/mocha.h>
#include <notifications/notifications.h>
#include <rpxloader/rpxloader.h>
#include <sdutils/sdutils.h>

#include <wups.h>

#include <coreinit/title.h>
#include <nn/spm.h>

#include <malloc.h>

WUPS_PLUGIN_NAME("Aroma Base Plugin");
WUPS_PLUGIN_DESCRIPTION("Base plugin template with utility functions.");
WUPS_PLUGIN_VERSION(PLUGIN_VERSION_FULL);
WUPS_PLUGIN_AUTHOR("Maschell");
WUPS_PLUGIN_LICENSE("GPL");

WUPS_USE_WUT_DEVOPTAB();
WUPS_USE_STORAGE("aroma_base_plugin"); // Unique id for the storage api

bool InitConfigValuesFromStorage() {
    bool result = true;
    WUPSStorageError storageError;
    auto subItemConfig = WUPSStorageAPI::GetOrCreateSubItem(CAT_CONFIG, storageError);
    if (!subItemConfig) {
        DEBUG_FUNCTION_LINE_ERR("Failed to get or create sub category \"%s\"", CAT_CONFIG);
        result = false;
    } else {
        if (subItemConfig->GetOrStoreDefault(USTEALTH_CONFIG_ID, gActivateUStealth, ACTIVATE_USTEALTH_DEFAULT) != WUPS_STORAGE_ERROR_SUCCESS) {
            DEBUG_FUNCTION_LINE_ERR("Failed to get or create item \"%s\"", USTEALTH_CONFIG_ID);
            result = false;
        }
        if (subItemConfig->GetOrStoreDefault(POWEROFFWARNING_CONFIG_ID, gSkip4SecondOffStatusCheck, SKIP_4_SECOND_OFF_STATUS_CHECK_DEFAULT) != WUPS_STORAGE_ERROR_SUCCESS) {
            DEBUG_FUNCTION_LINE_ERR("Failed to get or create item \"%s\"", POWEROFFWARNING_CONFIG_ID);
            result = false;
        }
        if (subItemConfig->GetOrStoreDefault(FORCE_NDM_SUSPEND_SUCCESS_CONFIG_ID, gForceNDMSuspendSuccess, FORCE_NDM_SUSPEND_SUCCESS_DEFAULT) != WUPS_STORAGE_ERROR_SUCCESS) {
            DEBUG_FUNCTION_LINE_ERR("Failed to get or create item \"%s\"", FORCE_NDM_SUSPEND_SUCCESS_CONFIG_ID);
            result = false;
        }
        if (subItemConfig->GetOrStoreDefault(ALLOW_ERROR_NOTIFICATIONS, gAllowErrorNotifications, ALLOW_ERROR_NOTIFICATIONS_DEFAULT) != WUPS_STORAGE_ERROR_SUCCESS) {
            DEBUG_FUNCTION_LINE_ERR("Failed to get or create item \"%s\"", ALLOW_ERROR_NOTIFICATIONS);
            result = false;
        }
    }



    if (WUPSStorageAPI::SaveStorage() != WUPS_STORAGE_ERROR_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("Failed to save storage");
        result = false;
    }

    return result;
}


INITIALIZE_PLUGIN() {
    initLogging();
    if (NotificationModule_InitLibrary() != NOTIFICATION_MODULE_RESULT_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("NotificationModule_InitLibrary failed");
    }
    if (RPXLoader_InitLibrary() != RPX_LOADER_RESULT_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("RPXLoader_InitLibrary failed");
    }
    if (SDUtils_InitLibrary() != SDUTILS_RESULT_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("SDUtils_InitLibrary failed");
    }

    if (Mocha_InitLibrary() != MOCHA_RESULT_SUCCESS) {
        DEBUG_FUNCTION_LINE_ERR("Mocha_InitLibrary failed");
    }

    InitConfigValuesFromStorage();

    InitConfigMenu();

    Utils::DumpOTPAndSeeprom();
}

ON_APPLICATION_START() {
    initLogging();
}

ON_APPLICATION_ENDS() {
    deinitLogging();
}

DEINITIALIZE_PLUGIN() {
    NotificationModule_DeInitLibrary();
    RPXLoader_DeInitLibrary();
    SDUtils_DeInitLibrary();
}

DECL_FUNCTION(uint32_t, SuspendDaemonsAndDisconnectIfWireless__Q2_2nn3ndmFv) {
    auto res = real_SuspendDaemonsAndDisconnectIfWireless__Q2_2nn3ndmFv();

    if (res != 0) {
        DEBUG_FUNCTION_LINE_ERR("SuspendDaemonsAndDisconnectIfWireless__Q2_2nn3ndmFv returned %08X", res);
        if (res == 0xA0B12C80 && gForceNDMSuspendSuccess) {
            DEBUG_FUNCTION_LINE_INFO("Patch SuspendDaemonsAndDisconnectIfWireless__Q2_2nn3ndmFv to return 0 instead of %08X", res);
            return 0;
        } else if (gAllowErrorNotifications) {
            NotificationModule_SetDefaultValue(NOTIFICATION_MODULE_NOTIFICATION_TYPE_ERROR, NOTIFICATION_MODULE_DEFAULT_OPTION_DURATION_BEFORE_FADE_OUT, 10.0f);
            NotificationModule_AddErrorNotification("\"nn::ndm::SuspendDaemonsAndDisconnectIfWireless\" failed. Connection to 3DS not possible");
        }
    }
    return res;
}

DECL_FUNCTION(int32_t, IsStorageMaybePcFormatted, bool *isPcFormatted, nn::spm::StorageIndex *index) {
    // Make sure the index is valid
    int32_t res = real_IsStorageMaybePcFormatted(isPcFormatted, index);
    if (gActivateUStealth && res == 0) {
        // always return false which makes the Wii U menu stop nagging about this drive
        *isPcFormatted = false;
    }

    return res;
}

DECL_FUNCTION(bool, MCP_Get4SecondOffStatus, int32_t handle) {
    if (gSkip4SecondOffStatusCheck) {
        return false;
    }

    return real_MCP_Get4SecondOffStatus(handle);
}

// Only replace for the Wii U Menu
WUPS_MUST_REPLACE_FOR_PROCESS(IsStorageMaybePcFormatted, WUPS_LOADER_LIBRARY_NN_SPM, IsStorageMaybePcFormatted__Q2_2nn3spmFPbQ3_2nn3spm12StorageIndex, WUPS_FP_TARGET_PROCESS_WII_U_MENU);
WUPS_MUST_REPLACE_FOR_PROCESS(MCP_Get4SecondOffStatus, WUPS_LOADER_LIBRARY_COREINIT, MCP_Get4SecondOffStatus, WUPS_FP_TARGET_PROCESS_WII_U_MENU);
WUPS_MUST_REPLACE(SuspendDaemonsAndDisconnectIfWireless__Q2_2nn3ndmFv, WUPS_LOADER_LIBRARY_NN_NDM, SuspendDaemonsAndDisconnectIfWireless__Q2_2nn3ndmFv);
