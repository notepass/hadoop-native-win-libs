/*
 * Compatibility shim for building Apache Hadoop winutils against Windows SDK
 * versions that declare GetFileInformationByName in fileapi.h.
 *
 * Hadoop's winutils.h declares its own GetFileInformationByName (returning
 * DWORD) inside an extern "C" block. The Windows SDK fileapi.h declares a
 * different GetFileInformationByName (returning BOOL, different parameters).
 * When both declarations appear in the same translation unit, MSVC emits
 * C2733 because you cannot have two extern "C" declarations of the same name.
 *
 * Fix: rename the SDK's version before fileapi.h is processed, by defining
 * GetFileInformationByName as a macro alias. fileapi.h's declaration then
 * becomes a declaration of the aliased name. After fileapi.h is done, we
 * undefine the macro so Hadoop's own declaration in winutils.h comes through
 * under its original name without conflict.
 *
 * This header must be force-included (/FI) before any other header so the
 * macro is in effect when fileapi.h is first processed.
 */
#pragma once

/* Rename the SDK symbol before fileapi.h declares it. */
#define GetFileInformationByName _SdkGetFileInformationByName

/* Pull in windows.h (which includes fileapi.h). The SDK declaration of
 * GetFileInformationByName will be parsed as _SdkGetFileInformationByName. */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

/* Remove the rename macro so Hadoop's own declaration passes through. */
#undef GetFileInformationByName
