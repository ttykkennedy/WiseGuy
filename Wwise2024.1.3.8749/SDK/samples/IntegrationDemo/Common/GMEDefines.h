/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2025 Audiokinetic Inc.
*******************************************************************************/

#pragma once

#if defined(__has_include) && !defined(INTDEMO_GME) && defined(AK_PLATFORM_SUPPORTS_GME)
// Enable GME demo if Tencent GME plugin is installed locally and if it is supported on the target platform
#if __has_include(<AK/Plugin/TencentGMEPlugin.h>)

#define INTDEMO_GME
#include <AK/Plugin/TencentGMEPlugin.h>
#if defined(AK_WIN) || defined(AK_PS4) || defined(AK_PS5) || defined(AK_ANDROID)
#pragma comment(lib, "TencentGMEPlugin")
#endif

#if defined(AK_IOS)
#pragma comment(lib, "gmesdk")
#pragma comment(lib, "iconv")
#pragma comment(lib, "resolv")
#elif defined(AK_PS4) || defined(AK_PS5)
#pragma comment(lib, "GME") // Need to copy or rename GME.a to libGME.a in Wwise/SDK/PS(VER)_SDK(VER)[ABI]/(CONFIGURATION/lib
#endif

#endif

#endif // __has_include
