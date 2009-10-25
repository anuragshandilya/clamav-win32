/*
 * Clamav Native Windows Port: memory related stuff
 *
 * Copyright (c) 2005-2008 Gianluigi Tiesi <sherpya@netfarm.it>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this software; if not, write to the
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _CWHELPERS_H_
#define _CWHELPERS_H_

#include <tlhelp32.h>
#include <psapi.h>

/* kernel32 */
typedef HANDLE (WINAPI *imp_CreateToolhelp32Snapshot)(DWORD, DWORD);
typedef BOOL (WINAPI *imp_Process32First)(HANDLE, PROCESSENTRY32 *);
typedef BOOL (WINAPI *imp_Process32Next)(HANDLE, PROCESSENTRY32 *);
typedef BOOL (WINAPI *imp_Module32First)(HANDLE, MODULEENTRY32 *);
typedef BOOL (WINAPI *imp_Module32Next)(HANDLE, MODULEENTRY32 *);
typedef HANDLE (WINAPI *imp_CreateRemoteThread)(HANDLE,
                LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD);

/* advapi32 */
typedef BOOL (WINAPI *imp_OpenProcessToken)(HANDLE, DWORD, PHANDLE);
typedef BOOL (WINAPI *imp_LookupPrivilegeValueA)(LPCSTR, LPCSTR, PLUID);
typedef BOOL (WINAPI *imp_AdjustTokenPrivileges)(HANDLE, BOOL, PTOKEN_PRIVILEGES, DWORD, PTOKEN_PRIVILEGES, PDWORD);

/* psapi */
typedef BOOL (WINAPI *imp_EnumProcessModules)(HANDLE, HMODULE *, DWORD, LPDWORD);
typedef BOOL (WINAPI *imp_EnumProcesses)(LPDWORD, DWORD, LPDWORD);
typedef DWORD (WINAPI *imp_GetModuleBaseNameA)(HANDLE, HMODULE, LPSTR, DWORD);
typedef DWORD (WINAPI *imp_GetModuleFileNameExA)(HANDLE, HMODULE, LPSTR, DWORD);
typedef DWORD (WINAPI *imp_GetModuleFileNameExW)(HANDLE, HMODULE, LPWSTR, DWORD);
typedef BOOL (WINAPI *imp_GetModuleInformation)(HANDLE, HMODULE, LPMODULEINFO, DWORD);

/* kernel32 */
typedef enum _CW_HEAP_INFORMATION_CLASS { CW_HeapCompatibilityInformation } CW_HEAP_INFORMATION_CLASS;

typedef BOOL (WINAPI *imp_HeapSetInformation)(HANDLE, CW_HEAP_INFORMATION_CLASS, PVOID, SIZE_T);
typedef BOOL (WINAPI *imp_ChangeServiceConfig2A)(SC_HANDLE, DWORD, LPVOID);

typedef BOOL (WINAPI *imp_IsWow64Process) (HANDLE, PBOOL);
typedef BOOL (WINAPI *imp_Wow64DisableWow64FsRedirection)(LPVOID *OldValue);
typedef BOOL (WINAPI *imp_Wow64RevertWow64FsRedirection)(LPVOID *OldValue);

/* dbghelp32 */
#ifdef _MSC_VER
#include <dbghelp.h>
#define MINDUMP_FLAGS (MINIDUMP_TYPE) \
    (MiniDumpWithDataSegs  | MiniDumpWithIndirectlyReferencedMemory | MiniDumpFilterModulePaths)
typedef BOOL (WINAPI *pMiniDumpWriteDumpFunc)(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE,
                                              CONST PMINIDUMP_EXCEPTION_INFORMATION,
                                              CONST PMINIDUMP_USER_STREAM_INFORMATION,
                                              CONST PMINIDUMP_CALLBACK_INFORMATION);

extern LONG __stdcall CrashHandlerExceptionFilter(EXCEPTION_POINTERS *pExPtrs);
#endif

typedef struct _kernel32_t
{
    BOOL ok;
    HINSTANCE hLib;
    imp_CreateToolhelp32Snapshot CreateToolhelp32Snapshot;
    imp_Process32First Process32First;
    imp_Process32Next Process32Next;
    imp_Module32First Module32First;
    imp_Module32Next Module32Next;
    imp_CreateRemoteThread CreateRemoteThread;

    /* optional */
    imp_HeapSetInformation HeapSetInformation;
    imp_Wow64DisableWow64FsRedirection Wow64DisableWow64FsRedirection;
    imp_Wow64RevertWow64FsRedirection Wow64RevertWow64FsRedirection;
    imp_IsWow64Process IsWow64Process;
} kernel32_t;

typedef struct _advapi32_t
{
    BOOL ok;
    HINSTANCE hLib;
    imp_OpenProcessToken OpenProcessToken;
    imp_LookupPrivilegeValueA LookupPrivilegeValueA;
    imp_AdjustTokenPrivileges AdjustTokenPrivileges;

    /* optional */
    imp_ChangeServiceConfig2A ChangeServiceConfig2A;
} advapi32_t;

typedef struct _psapi_t
{
    BOOL ok;
    HINSTANCE hLib;
    imp_EnumProcessModules EnumProcessModules;
    imp_EnumProcesses EnumProcesses;
    imp_GetModuleBaseNameA GetModuleBaseNameA;
    imp_GetModuleFileNameExA GetModuleFileNameExA;
    imp_GetModuleFileNameExW GetModuleFileNameExW;
    imp_GetModuleInformation GetModuleInformation;
} psapi_t;

typedef struct _helpers_t
{
    kernel32_t k32;
    advapi32_t av32;
    psapi_t psapi;
} helpers_t;

typedef int (*proc_callback)(PROCESSENTRY32 ProcStruct, MODULEENTRY32 me32, void *data);

#endif /* _CWHELPERS_H_ */
