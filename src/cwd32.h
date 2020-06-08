/*\
 * cwd32.cpp
 *
 * Copyright (c) 2020 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/
#ifndef _cwd32_h_
#define _cwd32_h_

#pragma warning( disable:4996 )
#include "cwd32Vers.h"

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <direct.h>

#define  Got_F		0x001
#define  Got_A		0x002   // 
#define  Got_O		0x004
#define  Got_S		0x008   // -sName = Add "Set Proj=Name" to output
#define  Got_C		0x010   // -cName = Add "Call Name" to output
#define  Got_V0	    0x020   // -v0 = Silent RUNNING ...
#define  Got_At	    0x040   // -@ = Add '@' to commands ... 2010-05-10 - default to -@+
#define  Got_AtU    0x080
#define  Got_8      0x100   // -8 = output DOS 8.3 name ...
#define  Got_B      0x200   // -b = bare, single line output ...
#define  Got_L		0x400   // -l = single line switch
#define  Not_SO	    0x8000  // NOT writing to standard out ...

#define  Cwd_FDef	Got_At

#define  VFH(a)      ( ( a > 0 ) && ( a != INVALID_HANDLE_VALUE ) )
#define  PEOR     "\n"      // for pintf outputs
#define  MEOR     "\r\n"    // CR + LF for file outputs (-wb mode)

// <ISWIN something> Makes it easier to determine appropriate code paths:
#if defined (WIN32)
#define IS_WIN32     TRUE
#else   // !WIN32
#define IS_WIN32     FALSE
#endif  // WIN32 y/n
#define IS_NT      IS_WIN32 && (BOOL)(GetVersion() < 0x80000000)
#define IS_WIN32S  IS_WIN32 && (BOOL)(!(IS_NT) && (LOBYTE(LOWORD(GetVersion()))<4))
#define IS_WIN95   (BOOL)(!(IS_NT) && !(IS_WIN32S)) && IS_WIN32

#endif // #ifndef _cwd32_h_
// eof - cwd32.h
