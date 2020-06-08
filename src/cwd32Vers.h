// cwd32Vers.h
#ifndef _cwd32Vers_h_
#define _cwd32Vers_h_
//;------------------------------------------------------------------------------
//;	Informatique Rouge
//;	Commenced 1989,90,91,...,94,...,96,...98,...2000,2001,...,2010... 2020
//;	Geoff R. McLane: e-mail: reports _AT_ geoffair _DOT_ info
//; Licence: GNU GPL version 2
//;------------------------------------------------------------------------------
//;	Modification Notes - inverted - ie grow upwards
#define  APP_VDATE   "7 June, 2020 (MSVC16.x64)"  // build in Dell03, with MSVC 16, 2019, x64
// APP_VDATE   "19 May, 2010 (MSVC8)"   // also add to drive, unless -@-, 
// APP_VDATE   "10 May, 2010 (MSVC8)"   // added default to adding -@, 
// and -@- to remove it. Also REMOVE StdAfx.h/StdAfx.cpp - ie NO predefined headers
// and moved from cwdVers.h to this cwd32Vers.h
// APP_VDATE   "16 June, 2007 (MSVC8)"   // added -b - bare CWD output, single line
// APP_VDATE   "27 February, 2007 (MSVC6)"   // remove single CR for NT
#undef	USE_ONLY_CR_FOR_NT	// this made trouble for wordpad editor, so REMOVE
// APP_VDATE   "12 January, 2005"   // add -8 display
// APP_VDATE   "8 February, 2002"   // fix cr/lf for WIN95, or just LF
// 16 December, 2001" MEOR  // various fixes

//; 03Sep98 Add switch for single line output                             Geoff.
//; 22Jan96 Produce ONLY DOS version, and add -v0 NO OUPUT except if
//;		error, and -@ to add '@' in front of commands ...       Geoff.
//; 19Mch94 Add -sxxxx to add Set PROJ=xxxx to output			grm
//;------------------------------------------------------------------------------
#define  CoName	"Informatique Rouge"
//;<"22 January, 1996">
//;<"3 September, 1998">
//;<"3 March, 2000">	; Just a re-build in UNIKA1
//;	It is a PIII/600MHZ running W98 SE - Geoff
// CurDate  "16 December, 2001"  // conversion to WIN32

// FEATURE SWITCHES
#define  AddS	   	0x00000001  //	; Add -sxxxx switch = Add "Set PROJ=xxxx" 
#define  AddC	   	0x00000002	// ; Add -cxxxx switch = Add "Call xxxx" to output
#define  AddV0	   	0x00000004  //	; Add -v0 switch = NO ouptut, except if error
#define  AddAt	      0x00000008  // ; Add -@ switch = Place this in front of commands
#define  AddL	      0x00000010  // ; Add -l to do output in a SINGLE line

#endif // #ifndef _cwd32Vers_h_
// eof - cwd32Vers.h
