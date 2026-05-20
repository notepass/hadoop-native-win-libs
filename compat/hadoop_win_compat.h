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
 * Fix: define GetFileInformationByName as a macro alias before windows.h is
 * included, so the SDK declaration lands under the alias (_SdkGetFileInformationByName).
 * Then #undef the macro so Hadoop's own declaration in winutils.h comes through
 * under the real name without conflict.
 *
 * Do NOT define WIN32_LEAN_AND_MEAN here — Hadoop code relies on the full
 * windows.h (including winioctl.h for FSCTL_GET_REPARSE_POINT etc.).
 *
 * This header is force-included (/FI via ForcedIncludeFiles in
 * Directory.Build.props) before any source file, so the macro rename is in
 * effect when fileapi.h is first processed.
 */
#pragma once
#define GetFileInformationByName _SdkGetFileInformationByName
#include <windows.h>
#undef GetFileInformationByName
