/*************************************************************************
    > File Name: def.h
    > Author: hsz
    > Brief:
    > Created Time: 2024年08月07日 星期三 13时14分59秒
 ************************************************************************/

#ifndef __EULAR_LOG2_DEF_H__
#define __EULAR_LOG2_DEF_H__

#if defined(_MSC_VER)
    #if defined(MAKE_SHARED) || defined(LOG_EXPORTS)
        #define LOG_EXPORT  __declspec(dllexport)
    #else
        #define LOG_EXPORT  __declspec(dllimport)
    #endif

    #define ATTR_FORMAT(x, y)
#elif defined(__GNUC__)
    #define LOG_EXPORT  __attribute__((visibility("default")))

    #define ATTR_FORMAT(fmt_index, var_begin) __attribute__((format(printf, fmt_index, var_begin)))
#else
    #define LOG_EXPORT
#endif

#ifdef __cplusplus

#ifndef EXTERN_C
#define EXTERN_C            extern "C"
#endif

#ifndef EXTERN_C_BEGIN
#define EXTERN_C_BEGIN      extern "C" {
#endif

#ifndef EXTERN_C_END
#define EXTERN_C_END        } // extern "C"
#endif

#else

#define EXTERN_C    extern
#define EXTERN_C_BEGIN
#define EXTERN_C_END

#endif

#endif // __EULAR_LOG2_DEF_H__
