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
 * Define GetFileInformationByName as a macro alias (Hadoop_GetFileInformationByName)
 * BEFORE windows.h is included. This renames BOTH the SDK declaration (in
 * fileapi.h) AND Hadoop's own declaration and definition everywhere this shim
 * is active. The original name is then owned solely by kernel32 and is never
 * referenced, so neither conflict occurs.
 *
 * The macro is intentionally NOT #undef'd — it must remain active throughout
 * each translation unit so that Hadoop's function definition and all call sites
 * consistently use the renamed symbol.
 *
 * This header is force-included (/FI via ForcedIncludeFiles in
 * Directory.Build.props) before any source file.
 */
#pragma once
#define GetFileInformationByName Hadoop_GetFileInformationByName
#include <windows.h>
