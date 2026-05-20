/*
 * Compatibility shim for building Apache Hadoop winutils against Windows SDK
 * versions that declare GetFileInformationByName in fileapi.h.
 *
 * Problem
 * -------
 * Newer Windows SDK fileapi.h declares:
 *   BOOL GetFileInformationByName(PCWSTR, FILE_INFO_BY_NAME_CLASS, PVOID, DWORD)
 * Hadoop winutils.h (line 129) declares its own:
 *   DWORD GetFileInformationByName(LPCWSTR, LPWIN32_FILE_ATTRIBUTE_DATA)
 * Both inside extern "C" => MSVC C2733 (cannot overload in extern "C").
 * Additionally, kernel32.lib exports GetFileInformationByName, so if Hadoop's
 * object file also defines it, the linker emits LNK2005.
 *
 * Fix
 * ---
 * Include windows.h first so the SDK declaration keeps the original symbol
 * name, then define GetFileInformationByName as a macro alias
 * (Hadoop_GetFileInformationByName). This renames Hadoop's declaration,
 * definition and call sites everywhere this shim is active.
 *
 * The macro is intentionally NOT #undef'd so each translation unit consistently
 * uses the renamed Hadoop symbol.
 *
 * This header is force-included (/FI via ForcedIncludeFiles in
 * Directory.Build.props) before any source file.
 */
#pragma once
#include <windows.h>
#define GetFileInformationByName Hadoop_GetFileInformationByName
