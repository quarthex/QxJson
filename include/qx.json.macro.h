/**
 * @file qx.json.macro.h
 * @brief Various preprocessor macros used by the project.
 * @author Romain DEOUX
 */

#ifndef _H_QX_MACRO
#define _H_QX_MACRO

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
