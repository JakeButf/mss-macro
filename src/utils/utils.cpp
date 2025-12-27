#include "utils.h"
#include "FSUtils.h"
#include "common.h"
#include "logger.h"

#include <mocha/mocha.h>

#include <coreinit/mcp.h>

#include <string>

#include <sys/stat.h>

bool Utils::GetSerialId(std::string &serialID) {
    bool result = false;
    alignas(0x40) MCPSysProdSettings settings{};
    auto handle = MCP_Open();
    if (handle >= 0) {
        if (MCP_GetSysProdSettings(handle, &settings) == 0) {
            serialID = std::string(settings.code_id) + settings.serial_id;
            result   = true;
        } else {
            DEBUG_FUNCTION_LINE_ERR("Failed to get SerialId");
        }
        MCP_Close(handle);
    } else {
        DEBUG_FUNCTION_LINE_ERR("MCP_Open failed");
    }
    return result;
}

/*
 * Dumps the OTP and SEEPROM when if it's no exists.
 */
void Utils::DumpOTPAndSeeprom() {
    std::string serialId;
    if (!Utils::GetSerialId(serialId)) {
        DEBUG_FUNCTION_LINE_WARN("Failed to get SerialId of the console, skip OTP/SEEPROM dumping");
        return;
    }
    std::string backupPathConsole            = string_format(BACKUPS_DIRECTORY_FULL "/%s", serialId.c_str());
    std::string backupPathConsoleOtpPath     = backupPathConsole + "/otp.bin";
    std::string backupPathConsoleSeepromPath = backupPathConsole + "/seeprom.bin";

    if (!FSUtils::CreateSubfolder(backupPathConsole.c_str())) {
        DEBUG_FUNCTION_LINE_WARN("Failed to create \"%s\"", backupPathConsole.c_str());
    }

    bool seepromExists = FSUtils::CheckFile(backupPathConsoleSeepromPath.c_str());
    bool otpExists     = FSUtils::CheckFile(backupPathConsoleOtpPath.c_str());

    if (!seepromExists) {
        uint8_t data[0x200] = {};
        if (Mocha_SEEPROMRead(data, 0, sizeof(data)) != sizeof(data)) {
            DEBUG_FUNCTION_LINE_WARN("Failed to read SEEPROM");
        } else {
            if (FSUtils::saveBufferToFile(backupPathConsoleSeepromPath.c_str(), (void *) data, sizeof(data)) != sizeof(data)) {
                DEBUG_FUNCTION_LINE_WARN("Failed to write SEEPROM backup (\"%s\")", backupPathConsoleSeepromPath.c_str());
            } else {
                DEBUG_FUNCTION_LINE_INFO("Created SEEPROM backup: \"%s\"", backupPathConsoleSeepromPath.c_str());
            }
        }
    } else {
        DEBUG_FUNCTION_LINE_VERBOSE("SEEPROM backup already exists");
    }
    if (!otpExists) {
        WiiUConsoleOTP otp = {};
        if (Mocha_ReadOTP(&otp) != MOCHA_RESULT_SUCCESS) {
            DEBUG_FUNCTION_LINE_WARN("Failed to read otp");
        } else {
            if (FSUtils::saveBufferToFile(backupPathConsoleOtpPath.c_str(), (void *) &otp, sizeof(otp)) != sizeof(otp)) {
                DEBUG_FUNCTION_LINE_WARN("Failed to write otp backup (\"%s\")", backupPathConsoleOtpPath.c_str());
            } else {
                DEBUG_FUNCTION_LINE_INFO("Created OTP backup: \"%s\"", backupPathConsoleOtpPath.c_str());
            }
        }
    } else {
        DEBUG_FUNCTION_LINE_VERBOSE("OTP backup already exists");
    }

    // Remove wrong file with typo
    std::string backupPathConsoleOTPPathTypo = backupPathConsole + "/opt.bin";
    if (FSUtils::CheckFile(backupPathConsoleOTPPathTypo.c_str())) {
        remove(backupPathConsoleOTPPathTypo.c_str());
    }
}