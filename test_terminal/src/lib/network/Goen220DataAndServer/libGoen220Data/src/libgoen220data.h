#pragma once

#include <QtGlobal>

#include <goen220_utils.h>
#include <goen220_tlm.h>
#include <goen220_tlm_data_model.h>
#include <goen220_types.h>
#include <goen220_cmd.h>



namespace Goen220 {
namespace Data {
/// @brief Возвращает строковое представление о версии библиотеки libGoen220Data
QString version();

/// @brief Возвращает мажорную-версию библиотеки libGoen220Data
unsigned int versionMajor();

/// @brief Возвращает минорную-версию библиотеки libGoen220Data
unsigned int versionMinor();

/// @brief Возвращает патч-версию библиотеки libGoen220Data
unsigned int versionPatch();
} // namespace Data
} // namespace Goen220
