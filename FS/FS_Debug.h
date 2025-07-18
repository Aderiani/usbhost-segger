/*********************************************************************
*                   (c) SEGGER Microcontroller GmbH                  *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*       (c) 2003 - 2021     SEGGER Microcontroller GmbH              *
*                                                                    *
*       www.segger.com     Support: www.segger.com/ticket            *
*                                                                    *
**********************************************************************
*                                                                    *
*       emUSB-Device * USB Device stack for embedded applications    *
*                                                                    *
*                                                                    *
*       Please note:                                                 *
*                                                                    *
*       Knowledge of this file may under no circumstances            *
*       be used to write a similar product.                          *
*                                                                    *
*       Thank you for your fairness !                                *
*                                                                    *
**********************************************************************
*                                                                    *
*       emUSB-Host version: V2.33-r27121                             *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File    : FS_Debug.h
Purpose : Debug macros for logging
-------------------------- END-OF-HEADER -----------------------------
*/

#ifndef FS_DEBUG_H                // Avoid recursive and multiple inclusion
#define FS_DEBUG_H

#include <stdarg.h>               //lint !e829 +headerwarn option was previously issued for header 'stdarg.h'. Reason: we need access to the va_start() and va_end() macros.
#include "SEGGER.h"
#include "FS_ConfDefaults.h"

#if defined(__cplusplus)
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
#ifndef   FS_DEBUG_LEVEL
  #define FS_DEBUG_LEVEL                    FS_DEBUG_LEVEL_CHECK_PARA      // For most targets, min. size is important
#endif

/*********************************************************************
*
*       Defines, fixed
*
**********************************************************************
*/

/*********************************************************************
*
*       Debug levels
*
*  Description
*    Permitted values for FS_DEBUG_LEVEL.
*
*  Additional information
*    The debug levels are hierarchical so that so that higher debug levels
*    also enable the features assigned to lower debug levels.
*/
#define FS_DEBUG_LEVEL_NOCHECK              0  // No run time checks are performed.
#define FS_DEBUG_LEVEL_CHECK_PARA           1  // Parameter checks are performed.
#define FS_DEBUG_LEVEL_CHECK_ALL            2  // Parameter checks and consistency checks are performed.
#define FS_DEBUG_LEVEL_LOG_ERRORS           3  // Error conditions are reported.
#define FS_DEBUG_LEVEL_LOG_WARNINGS         4  // Error and warning conditions are reported.
#define FS_DEBUG_LEVEL_LOG_ALL              5  // Error and warning conditions as well as trace messages are reported.

/*********************************************************************
*
*       FS_DEBUG_ERROROUT
*/
#if (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ERRORS)
  #define FS_DEBUG_ERROROUT(s)              FS_ErrorOutf s
#else
  #define FS_DEBUG_ERROROUT(s)
#endif

/*********************************************************************
*
*       FS_DEBUG_WARN
*/
#if (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_WARNINGS)
  #define FS_DEBUG_WARN(s)                  FS_Warnf s
#else
  #define FS_DEBUG_WARN(s)
#endif

/*********************************************************************
*
*       FS_DEBUG_LOG
*/
#if (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ALL)
  #define FS_DEBUG_LOG(s)                   FS_Logf s
#else
  #define FS_DEBUG_LOG(s)
#endif

#define EXPR2STRING(Expr)         #Expr   //lint !e9024 '#/##' operator used in macro 'FS_DEBUG_ASSERT' [MISRA 2012 Rule 20.10, advisory]. Rationale: This macro is used only in debug builds.

/*********************************************************************
*
*       FS_DEBUG_ASSERT
*/
#if (FS_DEBUG_LEVEL >= FS_DEBUG_LEVEL_LOG_ERRORS)
   #define FS_DEBUG_ASSERT(MType, Expr)              \
   if (!(Expr)) {                                    \
      FS_DEBUG_ERROROUT((MType, EXPR2STRING(Expr))); \
      FS_X_Panic(FS_ERRCODE_ASSERT_FAILURE);         \
    }
#else
  #define FS_DEBUG_ASSERT(MType, Expr)
#endif

/*********************************************************************
*
*       Logging (for debugging primarily)
*/
void FS_ErrorOutf     (U32 Type, const char * sFormat, ...);
void FS_Logf          (U32 Type, const char * sFormat, ...);
void FS_Warnf         (U32 Type, const char * sFormat, ...);

#if defined(__cplusplus)
  }              /* Make sure we have C-declarations in C++ programs */
#endif

#endif // FS_DEBUG_H

/*************************** End of file ****************************/
