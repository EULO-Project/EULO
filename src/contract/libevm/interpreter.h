/* Aleth: Ethereum C++ client, tools and libraries.
 * Copyright 2018 Aleth Autors.
 * Licensed under the GNU General Public License, Version 3. See the LICENSE file.
 */

#pragma once

#include <libevm/libevmc.h>

#if defined _MSC_VER || defined __MINGW32__
#define EVMC_EXPORT __declspec(dllexport)
#else
#define EVMC_EXPORT __attribute__((visibility("default")))
#endif

#if __cplusplus
#define EVMC_NOEXCEPT noexcept
#else
#define EVMC_NOEXCEPT
#endif

#if __cplusplus
extern "C" {
#endif

EVMC_EXPORT struct evmc_instance* evmc_create_interpreter() EVMC_NOEXCEPT;

#if __cplusplus
}
#endif
