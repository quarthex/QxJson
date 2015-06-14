/**
 * @file qx.json.macro.h
 * @brief Various preprocessor macros used by the project.
 * @author Romain DEOUX
 */

#ifndef _H_QX_MACRO
#define _H_QX_MACRO

/**
 * @def QX_API
 * @brief defines a way to declare API functions
 *
 * Defines QX_JSON_BUILD at build time to integrate the sources on another
 * project.
 */
#ifdef QX_JSON_BUILD
#  define QX_API
#else
#  ifdef __cplusplus
#    define QX_API extern "C"
#  else
#    define QX_API extern
#  endif
#endif

#endif /* _H_QX_MACRO */
