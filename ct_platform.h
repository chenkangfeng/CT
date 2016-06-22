//
//  ct_platform.h 2016-6-22
//

#ifndef _ct_platform_h_
#define _ct_platform_h_

#define ct_platform_unknown 0
#define ct_platform_ios     1
#define ct_platform_mac     2
#define ct_platform_android 3
#define ct_platform_win32   4
#define ct_platform_linux   5

#define ct_target_platform ct_platform_unknown

#if defined(CC_TARGET_OS_MAC) || defined(__APPLE__)
#undef  ct_target_platform
#define ct_target_platform ct_platform_mac
#endif

#if defined(CC_TARGET_OS_IPHONE)
#undef  ct_target_platform
#define ct_target_platform ct_platform_ios
#endif

#if defined(ANDROID)
#undef  ct_target_platform
#define ct_target_platform ct_platform_android
#endif

#if defined(_WIN32) && defined(_WINDOWS)
#undef  ct_target_platform
#define ct_target_platform ct_platform_win32
#endif

#if defined(LINUX) && !defined(__APPLE__)
#undef  ct_target_platform
#define ct_target_platform ct_platform_linux
#endif

#if ct_target_platform == ct_platform_unknown
#error "cannot recognize the target platform"
#endif

#endif // _ct_platform_h_
