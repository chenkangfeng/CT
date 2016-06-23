//
//  ctr_platform.h 2016-6-22
//

#ifndef _ctr_platform_h_
#define _ctr_platform_h_

#define ctr_platform_unknown 0
#define ctr_platform_ios     1
#define ctr_platform_mac     2
#define ctr_platform_android 3
#define ctr_platform_win32   4
#define ctr_platform_linux   5

#define ctr_target_platform ctr_platform_unknown

#if defined(CC_TARGET_OS_IPHONE)
#undef  ctr_target_platform
#define ctr_target_platform ctr_platform_ios
#endif

#if defined(CC_TARGET_OS_MAC) || defined(__APPLE__)
#undef  ctr_target_platform
#define ctr_target_platform ctr_platform_mac
#endif

#if defined(ANDROID)
#undef  ctr_target_platform
#define ctr_target_platform ctr_platform_android
#endif

#if defined(_WIN32) && defined(_WINDOWS)
#undef  ctr_target_platform
#define ctr_target_platform ctr_platform_win32
#endif

#if defined(LINUX) && !defined(__APPLE__)
#undef  ctr_target_platform
#define ctr_target_platform ctr_platform_linux
#endif

#if ctr_target_platform == ctr_platform_unknown
#error "unknown platform"
#endif

#endif // _ctr_platform_h_
