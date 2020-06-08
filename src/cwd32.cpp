/*\
 * cwd32.cpp
 *
 * Copyright (c) 2020 - Geoff R. McLane
 * Licence: GNU GPL version 2
 *
\*/
#include "cwd32.h"

typedef struct tagCNAME {
    void * link;
    char * call;
}CNAME, * PCNAME;

PCNAME  pCName = NULL;

DWORD    Cwd_Flag	=	Cwd_FDef;   // default FLAG(S)
INT      g_iVerbose = 1;
TCHAR    g_szFileName[264];
TCHAR    g_szTmpBuf[1024];
#define  VERB1    ( g_iVerbose > 0 )
#define  VERB5    ( g_iVerbose > 4 )
#define  VERB9    ( g_iVerbose > 8 )

#define  sprtf    printf

TCHAR szHelpMsg[] =
"Informatique Rouge *** CURRENT WORK DIRECTORY UTILITY 32 *** "
APP_VDATE
MEOR
" Purpose : Writes the current Drive and Directory to output" MEOR
" Usage   : CWD32 [FileName] [Switches]" MEOR
" Switches: Each preceded by / or - and space separated." MEOR
" -@[-|+]   Add an @ character to commands. Default is yes. -@- to not..." MEOR
" -a        Append to any existing file, if any." MEOR
" -b        Bare CWD, single line." MEOR
" -cName    Add 'Call Name' to output (Can be repeated)." MEOR
" -o        Overwrite any existing output file." MEOR
" -sName    Add 'Set PROJ=Name' to output." MEOR
" -v[0|9]   Silent or very verbal." MEOR
" -l        Single line output." MEOR
" -8        Output DOS 8.3 format." MEOR
" Notes   : A ? anywhere in the command will produce this help." MEOR;

static char * szCDate = __DATE__;
static char * szCTime = __TIME__;

int   iKey;
int   iWaitKey( void )
{
   int i;
   while( !_kbhit() )
   {
      // just wait for key
      // wait
   }
   // get key
   iKey = _getch();   // get the key
   i = toupper(iKey);   // return upper case only
   return i;
}

/* flags 
" file   Name of output file
#define  Got_F		   1
" -a        Append to any existing file, if any." MEOR
#define  Got_A			2
" -o[File]  Overwrite any existing file. [Optional out file]" MEOR
#define   Got_O		   4
" -sName    Add 'Set PROJ=Name' to output." MEOR
#define  Got_S			8  //	; -sName = Add "Set Proj=Name" to output
" -cName    Add 'Call Name' to output (Can be repeated)." MEOR
#define  Got_C		0x10  // h	; -cName = Add "Call Name" to output
#define  Got_V0	0x20  // h	; -v0 = Silent RUNNING ...
" -@        Add an @ character to commands." MEOR
#define  Got_At	0x40  // h	; -@ = Add '@' to commands ...
" -l        Single line output." MEOR
#define  Got_L		0x80  // h	; -l = single line switch
" -v0       Silent running." MEOR
#define  Not_SO	0x8000   // h	; NOT writing to standard out ...
 === */

void Kill_CNAME_list(void)
{
    PCNAME pn = pCName;
    if ( pn )
    {
        PCNAME nxt = (PCNAME)pn->link;
        while(nxt)
        {
            free(pn->call);
            delete pn;
            pn = nxt;
            nxt = (PCNAME)pn->link;
        }
        free(pn->call);
        delete pn;
    }
}

void Add_PCNAME_list( PCNAME pcn )
{
    pcn->link = NULL;
    PCNAME pn = pCName;
    if ( pn )
    {
        PCNAME nxt = (PCNAME)pn->link;
        while(nxt)
        {
            pn = nxt;
            nxt = (PCNAME)pn->link;
        }
        pn->link = pcn;
    }
    else
        pCName = pcn;
}

int Add_CNAME( char * cp )
{
    int iret = 0;
    if (strlen(cp))
    {
        PCNAME pcn = new CNAME;
        if(pcn)
        {
            pcn->call = strdup(cp);
            Add_PCNAME_list(pcn);
        }
        else
        {
            sprtf("ERROR: Memory allocation FAILED!" PEOR);
            iret = 1;
        }
    }
    return iret;
}


int  preprocesscmd( HANDLE hout, int argc, char * argv[] )
{
   LPTSTR   lph = szHelpMsg;
//   DWORD    dww;
   int   i, c;
   char *   cp;
   LPTSTR   lpf = &g_szFileName[0];
   *lpf = 0;
   for( i = 1; i < argc; i++ )
   {
      c = 0;
      cp = argv[i];
      if(cp)
         c = *cp;    // get the command char
      if( ( c == '-' ) || ( c == '/' ) )
      {
         // we have a switch
         cp++;
         c = toupper(*cp);
         switch(c)
         {
         case '8': // -8 = Output 8.3 name
            Cwd_Flag |= Got_8;
            break;
         case 'A':   //" -a        Append to any existing file, if any." MEOR
            Cwd_Flag |= Got_A;
            break;
         case 'B':   //" -b        Bare, single line output of CWD
            Cwd_Flag |= Got_B;
            break;
         case 'O':
            // " -o  Overwrite any existing file." MEOR
            Cwd_Flag |= Got_O;
            break;
         case 'S':
            // " -sName    Add 'Set PROJ=Name' to output." MEOR
            //Cwd_Flag |= Got_S;   //	; -sName = Add "Set Proj=Name" to output
            break;
         case 'C':
            // " -cName    Add 'Call Name' to output (Can be repeated)." MEOR
            Cwd_Flag |= Got_C;  // -cName = Add "Call Name" to output
            cp++;
            if (*cp)
                Add_CNAME(cp);
            else
            {

            }

            break;

         case 'V':
            cp++;    // goto next
            c = toupper(*cp);
            if( c )
            {
               if( ( c >= '0' ) && ( c <= '9' ) )
               {
                  // got a number
                  g_iVerbose = (c - '0');
                  if( c == '0' )
                  {
                     Cwd_Flag |= Got_V0;  // -v0 = Silent RUNNING
//" -v0       Silent running." MEOR
// #define  Not_SO	0x8000   // h	; NOT writing to standard out ...
                  }
               }
               else
               {
                  cp -= 2;
                  sprtf( "ERROR: Only -v[0-9] allowed! Got [%s]?" MEOR, cp );
                  return (int)-5;
               }
            }
            else
               g_iVerbose = 1;
            break;

         case '@':
            // " -@        Add an @ character to commands." MEOR
            Cwd_Flag |= (Got_At | Got_AtU);  //	; -@ = Add '@' to commands ...
            // FIX2010 - Default to add '@', and add -@[+|-] On|Off Y[es]|N[o]...
            cp++;    // goto next
            c = toupper(*cp);
            if (c) {
                if ((c == '-')||(c == 'N')||(stricmp(cp,"off")==0))
                    Cwd_Flag &= ~(Got_At);  //	REMOVE FLAG
                else if ((c == '+')||(c == 'Y')||(stricmp(cp,"on")==0))
                    Cwd_Flag |= Got_A;
                else
                {
                    cp -= 2;
                    sprtf( "ERROR: Only -@[+|-] allowed! Or No,Yes,On,Off... Got [%s]?" MEOR, cp );
                    return (int)-6;
                }
            }
            break;
         case 'L':
            //" -l        Single line output." MEOR
            Cwd_Flag |= Got_L;   //; -l = single line switch
            break;

         default:
            cp--;
            // SHOW HELP
            //WriteFile( hout, lph, strlen(lph), &dww, NULL );
            sprtf(lph);
            sprintf(lph," Compiled on %s, at %s" MEOR, szCDate, szCTime);
            sprtf(lph);
            // and then the BAD COMMAND
            sprtf( "ERROR: Invalid command switch! [%s]?" PEOR, cp );
            //exit(-4);
            return (int)-4;
            break;
         }
      }
      else
      {
         if( *lpf )
         {
            sprtf( "ERROR: Only one output file can be given!" PEOR );
            //exit(-3);
            return (int)-3;
         }
         strcpy(lpf, cp);  // get the out file name
         Cwd_Flag	|=	Got_F;   // signal we have an OUTPUT file
      }
   }

   if( VERB9 )
   {
      LPTSTR   lpb = g_szTmpBuf;
      sprintf( lpb, "CMD(%d):", (argc - 1) );
      for( i = 1; i < argc; i++ )
      {
         c = 0;
         cp = argv[i];
         if(cp)
         {
            if( ( strlen(lpb) + strlen(cp) ) > 1024 )
            {
               exit(1024);
            }
            else
            {
               strcat(lpb, " ");
               strcat(lpb, cp);
               //sprtf(cp);    // get the command char
            }
         }
      }

      strcat(lpb, PEOR);
      sprtf(lpb);
   }
   return 0;
}


void FixAlternateFileName( WIN32_FIND_DATA * pfd )
{
   DWORD dwi = (DWORD)strlen( &pfd->cFileName[0] );
   if( dwi && ( dwi < 14 ) )
   {
      LPTSTR lpf = &pfd->cFileName[0];
      LPTSTR lpa = &pfd->cAlternateFileName[0];
      for( dwi = 0; dwi < 14; dwi++ )
      {
         *lpa = (TCHAR)toupper( *lpf );
         if( *lpf == 0 )
            break;
         lpf++;
         lpa++;
      }
      *lpa = 0;
   }
   if( VERB9 )
   {
      sprtf( "NOTE: FIXUP Alt. NAME=[%s] of [%s]!" MEOR,
         &pfd->cAlternateFileName[0],
         &pfd->cFileName[0] );
   }
}

// simple - any input is taken to be an OUTPUT file
int main(int argc, char* argv[])
{
   int      iret;
   int      i;
   LPTSTR   lpd, lpcmd, lp8;
   LPTSTR   lpf;
   HANDLE   hout, hso;
   LONG     dww;
   WIN32_FIND_DATA   fd;
   HANDLE   hFind; // = FindFirstFile( lpf, &fd );

	//sprtf("Hello World!\n");
   // Save the handle to the current STDOUT.
   hout = GetStdHandle(STD_OUTPUT_HANDLE);
   hso = hout;
   //hSaveStderr = GetStdHandle(STD_ERROR_HANDLE);
   iret = preprocesscmd( hout, argc, argv );
   if( iret )
      return iret;

   lpd = new TCHAR[2048];
   if( !lpd )
   {
      sprtf("ERROR: Get memory FAILED!" PEOR );
      return (int) -1;
   }

   lpf = &g_szFileName[0];
   lpcmd = &lpd[1024];
   lp8 = &lpcmd[512];
   if( ( argc > 1 ) && ( *lpf ) )
   {
      //lpf = argv[1]; // get first argument
      //WIN32_FIND_DATA   fd;
      hFind = FindFirstFile( lpf, &fd );
      i = 'Y'; // set a YES
      if( VFH(hFind) )
      {
         // we ARE overwriting a FILE
         if( !(Cwd_Flag & Got_O) )
         {
            sprtf( "WARNING: File [%s] exits!" PEOR
               "*** [O]VERWRITE ***, [A]ppend, else no :", lpf );
            i = iWaitKey();
            if( i == 'O' )
            {
               i = 'Y';
               sprtf( "Overwrite" PEOR );
            }
            else if( i == 'A' )
            {
               i = 'Y';
               Cwd_Flag |= Got_A;
               sprtf( "Append" PEOR );
            }
            else
            {
               sprtf( "No file write" PEOR );
            }
         } else {
            if( VERB9 )
            {
               sprtf( "WARNING: OVERWRITING File [%s]!" MEOR, lpf );
            }
         }
         FindClose( hFind );
      }
      else if( VERB9 )
      {
         sprtf( "File [%s] does NOT exist." PEOR, lpf );
      }

      if( i == 'Y' )
      {
         HANDLE hFile = 0; // set NO handle
         if( Cwd_Flag & Got_A )
         {
            if( VERB9 )
               sprtf( "Append to file [%s]." PEOR, lpf );

            hFile = CreateFile(  lpf, // file name
               GENERIC_READ | GENERIC_WRITE,  // access mode
               0,  // share mode
               0, // SD
               OPEN_EXISTING, // how to create
               FILE_ATTRIBUTE_NORMAL,  // file attributes
               0 ); // handle to template file
            if(VFH(hFile))
            {
               dww = 0;
               SetFilePointer( hFile, 0, &dww, FILE_END );
            }
         }

         if( !VFH(hFile) )
         {
            // try to CREATE the file
            hFile = CreateFile( lpf,   // file name
               GENERIC_READ | GENERIC_WRITE,                      // access mode
               0,                          // share mode
               0, // SD
               CREATE_ALWAYS,                // how to create
               FILE_ATTRIBUTE_NORMAL,                 // file attributes
               0 );        // handle to template file
         }

         if( VFH(hFile) )
         {
            if( VERB9 )
               sprtf( "Set file [%s] as output on handle %#x." PEOR, lpf, PtrToInt(hFile) );
            hout = hFile;
//	mov	[Act_File],ax		; Keep the FILE HANDLE
            Cwd_Flag |= Not_SO;  // Signal NOT standard out
         }
         else
         {
            sprtf( "ERROR: Unable to create [%s]!" PEOR, lpf );
            hout = 0;
         }
      }
   }
   else if( VERB9 )
      sprtf( "*** NO COMMAND ***" PEOR ); // will never actually happen

   // *** THIS IS WHAT IT IS ALL ABOUT ***
   // ************************************
   if( _getcwd( lpd, 256 ) )
   {
      DWORD ilen = (DWORD)strlen(lpd);
      if( Cwd_Flag & Got_8 ) {
         // zeeks, output is to be 8.3 format
         DWORD i, c, d;
         WIN32_FIND_DATA * pfd = &fd;
         *lp8 = 0; // clear accumulation buffer
         d = 0;
         for( i = 0; i < ilen; i++ ) {
            c = lpd[i];
            if(( c == '\\' ) && (d != ':')) {
               // got a FOLDER
               lpcmd[i] = 0;
               ZeroMemory(pfd, sizeof(WIN32_FIND_DATA));
               hFind = FindFirstFile( lpcmd, pfd );
               if( VFH(hFind) ) {
                  strcat(lp8,"\\");
                  FindClose(hFind);
                  if( pfd->cAlternateFileName[0] == 0 ) {
                     FixAlternateFileName(pfd);
                  }
                  strcat(lp8, fd.cAlternateFileName);
               } else {
                  break;
               }
            }
            lpcmd[i] = (TCHAR)c;
            d = c; // keep PREVIOUS
         }
         if( i == ilen ) {
            // we made it, now construct the 8.3 output
            PTSTR p = strchr(lpd,':');
               // do last FOLDER
               lpcmd[i] = 0;
               ZeroMemory(pfd, sizeof(WIN32_FIND_DATA));
               hFind = FindFirstFile( lpcmd, pfd );
               if( VFH(hFind) ) {
                  strcat(lp8,"\\");
                  FindClose(hFind);
                  if( pfd->cAlternateFileName[0] == 0 ) {
                     FixAlternateFileName(pfd);
                  }
                  strcat(lp8, fd.cAlternateFileName);
               } else {
                  // break;
               }
            i = 0;
            if(p) {
               p++;
               i = (DWORD)(p - lpd);
               //*p = 0;
            }
            lpd[i] = 0;
            strcat(lpd,lp8); // put in the 8.3 format
         }
      }

      if( VFH(hout) )
      {
         DWORD    dww;
         LPTSTR   lpd2 = lpd;
         *lpcmd = 0;
         if( VERB9 )
               sprtf( "CWD = [%s]." PEOR, lpd );

         if( !( Cwd_Flag & (Got_L | Got_B) ) )   //; -l = single line switch
         {
            if( ( ilen >= 2 ) && ( lpd2[1] == ':' ) )
            {
               //if(( Cwd_Flag & Got_At )&&( Cwd_Flag & Got_AtU ))  // -@ = Add '@' to commands ...
               if ( Cwd_Flag & Got_At )  // 2010-05-19 - is default action, unless -@-
               {
                  strcpy(lpcmd, "@");
                  strcat(lpcmd, lpd2);
                  lpcmd[3] = 0;
               }
               else
               {
                  strcpy(lpcmd, lpd2);
                  lpcmd[2] = 0;
               }
#ifdef	USE_ONLY_CR_FOR_NT
               if( IS_NT )
               {
                  strcat(lpcmd, PEOR);
               }
               else
               {
                  strcat(lpcmd, MEOR);
               }
#else // !#ifdef	USE_ONLY_CR_FOR_NT
               strcat(lpcmd, MEOR);
#endif	// #ifdef	USE_ONLY_CR_FOR_NT y/n

               lpd2 += 2;
               ilen -= 2;
               WriteFile( hout, lpcmd, (DWORD)strlen(lpcmd), &dww, NULL );
            }
         }

         *lpcmd = 0;
         if( Cwd_Flag & Got_At )  //	; -@ = Add '@' to commands ...
         {
             if (Cwd_Flag & Got_B)
             {
                 if (Cwd_Flag & Got_AtU)
                     strcat(lpcmd, "@");
             }
             else
                 strcat(lpcmd, "@");
         }
         if( !(Cwd_Flag & Got_B) )
            strcat( lpcmd, "CD " );

         if( strchr( lpd2, ' ' ) )  // contains spacey chars
         {
            // encase in double inverted commas
            strcat(lpcmd, "\"");
            strcat(lpcmd, lpd2);
            strcat(lpcmd, "\"");
         }
         else
         {
            strcat(lpcmd, lpd2);
         }

#ifdef	USE_ONLY_CR_FOR_NT
         if( IS_NT )
         {
            strcat(lpcmd, PEOR); // let's try without, but should add it if running
            // NT 4.0!!!
         }
#else // !#ifdef	USE_ONLY_CR_FOR_NT
         strcat(lpcmd, MEOR); // let's try with! CR and LF
#endif // #ifdef	USE_ONLY_CR_FOR_NT y/n
         if (( Cwd_Flag & Got_C ) && pCName)
         {
             PCNAME pn,nxt;
             pn = pCName;
             if( Cwd_Flag & Got_At )  //	; -@ = Add '@' to commands ...
                strcat(lpcmd, "@");
             strcat(lpcmd,"call ");
             strcat(lpcmd,pn->call);
             strcat(lpcmd,MEOR);
             nxt = (PCNAME)pn->link;
             while (nxt)
             {
                 pn = nxt;
                 if( Cwd_Flag & Got_At )  //	; -@ = Add '@' to commands ...
                    strcat(lpcmd, "@");
                 strcat(lpcmd,"call ");
                 strcat(lpcmd,pn->call);
                 strcat(lpcmd,MEOR);
                 nxt = (PCNAME)pn->link;
             }
         }

         WriteFile( hout, lpcmd, (DWORD)strlen(lpcmd), &dww, NULL );

         if( hout != hso )
         {
            CloseHandle(hout);   // shut the door
         }
      }
//      else  // *** NO hout HANDLE ***
//      {

//      }
   }
   else
   {
      sprtf( "ERROR: Failed to get current working directory!" PEOR );
      iret = 0;
   }

   delete   lpd;

    Kill_CNAME_list();
//   i = iWaitKey();

	return iret;
}

/* the ASM
.xlist
;=======================================================================
.DATA
;=======================================================================
m_crlf	db	0dh,0ah
def_cwd	label	byte
	db	'a:'
	db	0dh,0ah
	db	'cd \'
lm_cwd	equ	$ - def_cwd
Len_DefCmd	label word
	dw	lm_cwd		; Length of DEFAULT (-l will change it)
	If	AddS
def_pn_at	label byte
	db	'@'
def_pn	label byte
	db	'Set PROJ='
ldef_pn	equ	$ - def_pn
	Endif				; AddS
	If	AddC
def_cn_at	label byte
	db	'@'
def_cn	label byte
	db	'Call '
ldef_cn	equ	$ - def_cn
	Endif				; AddC
ov_msg	db	'Warning - File $'
ov2_msg	db	' exists. Overwrite Y/N (Def=N) -> $'
;=======================================================================
.CODE
;=======================================================================
BEGIN_TEST:
	test	[Cwd_Flag],Got_F	; Save ON
	jz	Dn_Save_File
	call	Get_Cwd_File
Dn_Save_File:
	Call	Out_CWD
	If	AddS
	Test	[Cwd_Flag],Got_S	; Do we hv a PROJECT name?
	Jz	Dn_S_Name
	Call	Add_S_Name
Dn_S_Name:
	Endif				; AddS
	If	AddC
	Test	[Cwd_Flag],Got_C	; Do we hv a PROJECT name?
	Jz	Dn_C_Name
	Call	Add_C_Name		; Append (maybe multiple) "Call xx"
Dn_C_Name:
	Endif				; AddC
	call	Close_Save_File
; Note: The above call returned [Act_File] to 1 (if otherwise)
;=======================================|
	Test	[Cwd_Flag],Got_F		; Were we given a file?
	Jz	Dn_Fi_Adv
	Call	show_cwd_file
Dn_Fi_Adv:
	XOR	AL,AL
Pgm_Exit:
	DOS_EXIT
show_cwd_file:
	Test	[Cwd_Flag],Got_V0
	Jz	@F
	Ret
@@:	dmsg	<'Output written to file '>
	D2Nul	cwd_file
	Ret
addchr	macro
	mov	es:[di],al
	inc	di
	inc	dx
	endm
;	RemoveCrLf
;	Input:	DS:DX -> output item to be written
;		CX    = length
RemoveCrLf:
	cmp	cx,( 4 + 3 )
	ja	@F
	ret
@@:	push	ds
	pop	es
	mov	di,dx
	add	di,2
	mov	si,di
	add	si,( 2 + 3 )
	mov	bx,cx
	sub	cx,(( 4 + 3 ) - 1 )	; NOTE: Also move the NUL terminator
	cld
	rep	movsb
	mov	cx,bx
	sub	cx,( 2 + 3 )
	sub	[Len_DefCmd],( 2 + 3 )
	ret
Out_CWD:
	mov	dx,offset m_cwd
;;;	mov	cx,lm_cwd
	mov	cx,[Len_DefCmd]		; get its length
	add	cx,[DefSDir_Len]
	If	AddL
	Test	[Cwd_Flag],Got_L	; Single line output ...
	Jz	Out_CWD1		; NO
	Call	RemoveCrLf		; Yes
Out_CWD1:
	EndIf	; AddL - single line output
	mov	bx,[Act_File]		; = 1 for std out
	Test	[Cwd_Flag],Got_At	; Do we ADD an '@' character ...
	Jnz	Out_CWD2
	DOS_WRITE
Out_CWD_End:
	file_crlf			; Add Cr/Lf
	Ret
Out_CWD2:
	push	es
	LEs	Di,[Bgn_Ptr]	; Buffer for output
	Mov	Si,Dx		; Get current message ...
	sub	dx,dx		; Keep count of output ...
	mov	al,'@'
	addchr
Out_CWLp:
	mov	al,[Si]
	inc	si
	addchr
	cmp	al,0ah
	jne	@f
	mov	al,'@'
	addchr
@@:	loop	Out_CWLp
	mov	cx,dx
	push	ds
	lds	dx,[Bgn_Ptr]
	DOS_WRITE
	pop	ds	
	pop	es
	Jmp	Out_CWD_End			; Add Cr/Lf
Give_Help:
	Mov	Dx,offset hlp_msg
	DOS_PRT
	Xor	Al,Al
	Jmp	Pgm_Exit
Get_Cwd_File:
	mov	dx,offset cwd_file
	mov	cx,0			; No attrintues yet
	mov	al,2			; r/w
	test	[Cwd_Flag],Got_A	; Append to ...
	Jnz	Get_Cwd_F0
	test	[Cwd_Flag],Got_O	; File OVERWRITE
	jnz	Get_cwd_F2		; Yes - No overwrite check
Get_Cwd_F0:
	DOS_OPEN			; Else OPEN IT first
	Jc	Get_cwd_F2		; ok - does not exit
	Mov	Bx,Ax
	Test	[Cwd_Flag],Got_A	; DO we hv an append request?
	jz	Chk_4_Ovr
	Call	Mov_2_End
	Jmp	Got_Cwd_File	
Chk_4_Ovr:
	DOS_CLOSE
	Call	Warn_Over_Wrt
	Je	Get_cwd_F1
	Ret
Get_cwd_F1:
Get_cwd_F2:
	mov	dx,offset cwd_file
	mov	cx,0			; No attrintues yet
	mov	al,2			; r/w
	DOS_CREATE
	jnc	Got_Cwd_File
	Jmp	Bad_Cwd_File
Got_Cwd_File:
	mov	[Act_File],ax		; Keep the FILE HANDLE
	or	[Cwd_Flag],Not_SO	; Signal NOT standard out ...
	xor	ax,ax
	ret
Mov_2_End:
	sub	cx,cx
	sub	dx,dx
	mov	al,2
	DOS_MOVEPTR
	Jnc	@F
	Jmp	Bad_Mov_File
@@:
	Mov	FileSiz1,Ax
	Mov	FileSiz2,Dx
	mov	ax,bx
	Ret
Warn_Over_Wrt:
	push4
	Mov	Dx,Offset ov_msg
	DOS_PRT
	Mov	Si,offset cwd_file
sh_nx_ch:
	lodsb
	or	al,al
	jz	dn_fil_show
	putchr	al
	jmp	sh_nx_ch
dn_fil_show:
	Mov	Dx,offset ov2_msg
	DOS_PRT
	Call	Get_Key
	Cmp	Al,'Y'
	Je	got_a_rep
	And	[Cwd_Flag],NOT Got_F	; Remove the OUTPUT file ...
	Mov	Al,'N'
got_a_rep:
	putchr	al
	do_crlf			; put cr/lf
	Cmp	Al,'Y'		; set EXIT condition
	pop4
	Ret
Get_Key:
	Mov	Ah,1
	Call	BIOS_16
	Jz	Get_Key
	Mov	Ah,0
	Call	BIOS_16
	upit
	Ret
Close_Save_File:
	mov	bx,[Act_File]
	cmp	bx,2
	ja	Close_File
	ret
Close_File:
	DOS_CLOSE
	mov	[Act_File],1
	ret
Chk_SW:	cmp	al,'-'
	je	Got_SW
	cmp	al,'/'
;	je	Got_SW
Got_SW:
	Ret
Dec_Cmd:
	Get_Cmd
	jnz	chk_cmd
cmd_ret:
	ret			; ZF=Good NZ=Bad
chk_cmd:
	cmp	al,' '
	jbe	Dec_Cmd
got_cmd:
	Call	Chk_SW
	Jne	Got_Out_File
	Jmp	Chk_Switch
Got_Out_File:
	Test	[Cwd_Flag],Got_F
	Jz	Got_Out_1
	Jmp	Dup_Out_File
Got_Out_1:
	Mov	Di,Offset cwd_file
	Or	[Cwd_Flag],Got_F
Get_SF2:
	mov	[Di],Al
	inc	Di
	Get_Cmd
	jz	Got_SF10
	cmp	al,' '
	ja	Get_SF2
Got_SF10:
	Xor	Ax,Ax
	Mov	[Di],Ax
	Jmp	Dec_Cmd			; Back for more
Chk_Switch:
	Get_Cmd
	Jnz	@F
	Jmp	Bad_Command
@@:
	cmp	al,'O'		; Do we have a Save
	jne	Not_O
	Or	[Cwd_Flag],Got_O
	Jmp	Dec_Cmd
Not_O:
	Cmp	Al,'A'
	Jne	Not_A
	Or	[Cwd_Flag],Got_A
	Jmp	Dec_Cmd
Not_A:
	If	AddS
	Cmp	Al,'S'
	Jne	Not_S
	Jmp	Get_S_Name
Not_S:
	EndIf			; AddS
	If	AddC
	Cmp	Al,'C'
	Jne	Not_C
	Jmp	Get_C_Name
Not_C:
	EndIf			; AddC
	If	AddV0
	Cmp	Al,'V'
	Jne	Not_V
	Get_Cmd
	Jnz	@F
Bad_V:	Jmp	Bad_Command
@@:	Cmp	Al,'0'
	Jne	Bad_V
	Or	[Cwd_Flag],Got_V0	; Set SILENT
	Jmp	Dec_Cmd
Not_V:
	EndIf			; AddV0
	If	AddAt
	Cmp	Al,'@'
	Jne	Not_At
	Or	[Cwd_Flag],Got_At	; Add an '@' in front of commands ...
	Jmp	Dec_Cmd
Not_At:
	EndIf			; AddAt
	If	AddL	; Add -l to do output in a SINGLE line
Chk_L:	Cmp	Al,'L'
	Jne	Not_L
	Or	[Cwd_Flag],Got_L ; single line output ...
	Jmp	Dec_Cmd
Not_L:
	EndIf			; AddL
	jmp	Bad_Command
	If	AddS
Get_S_Name:
	Get_Cmd
	Jnz	@F
	Jmp	Bad_Command
@@:
	cmp	al,' '
	Jbe	Get_S_Name
	mov	di,offset S_Name	; Area for 'proj' save
	sub	dx,dx
Add_2_Name:
	mov	[di],ah			; Add it per the input
	inc	di			; that is NOT UPPER CASE ONLY
	inc	dx
	Get_Cmd
	jz	End_S_Name
	Cmp	Al,' '
	Ja	Add_2_Name
End_S_Name:
	Sub	Ax,Ax
	Mov	[Di],Ax
	Mov	PN_Len,Dx
	Or	[Cwd_Flag],Got_S	; Show we have a PROJ name
	Jmp	Dec_Cmd
Add_S_Name:
	mov	dx,offset def_pn
	mov	cx,ldef_pn
	mov	bx,[Act_File]		; = 1 for std out
	Test	[Cwd_Flag],Got_At
	jz	@F
	dec	dx
	inc	cx
@@:	DOS_WRITE
	mov	dx,offset S_Name
	mov	cx,PN_Len
	mov	bx,[Act_File]		; = 1 for std out
	DOS_WRITE
	file_crlf			; Add Cr/Lf
	Ret
	Endif			; AddS
	If	AddC
Get_C_Name:
	Get_Cmd
	Jnz	@F
	Jmp	Bad_Command
@@:
	cmp	al,' '
	Jbe	Get_C_Name
	mov	di,offset C_Name	; Area for 'proj' save
	Test	[Cwd_Flag],Got_C	; Do we already have a CALL command
	jnz	@F
	mov	CN_Len,0
@@:	
	mov	dx,CN_Len
	Add	Di,Dx			; Bump to next location
	push	es
	push	ds
	pop	es
	mov	si,offset def_cn
	mov	cx,ldef_cn
	test	[Cwd_Flag],Got_At
	Jz	@F
	dec	si
	inc	cx
@@:	mov	dx,cx
	cld
	rep	movsb			; Move in the 'CALL '
	pop	es			; or '@Call ' if Add_At (-@ switch)
@@:	mov	[di],ah			; Add it per the input
	inc	di			; that is NOT UPPER CASE ONLY
	inc	dx
	Get_Cmd
	jz	End_C_Name
	Cmp	Al,' '
	Ja	@B
End_C_Name:
	mov	al,0dh
	mov	[di],al
	inc	di
	inc	dx
	mov	al,0ah
	mov	[di],al
	inc	di
	inc	dx
	Add	CN_Len,Dx	; Add in NEW length
	Or	[Cwd_Flag],Got_C	; Show we have a CALL name
	Jmp	Dec_Cmd
Add_C_Name:
	mov	dx,offset C_Name
	mov	cx,CN_Len
	mov	bx,[Act_File]		; = 1 for std out
	DOS_WRITE
	Ret
	Endif			; AddC
Dup_Out_File:
	dmsg	<'Error - Already have output file '>
	D2Nul	cwd_file
	Mov	Al,1
	Jmp	Err_Exit
Bad_Cwd_File:
	dmsg	<'Error - Unable to create '>
	D2Nul	cwd_file
	Jmp	Err_Exit
Bad_Mov_File:
	dmsg	<'Error - Unable to append to '>
	D2Nul	cwd_file
	Jmp	Err_Exit
Bad_Command:
	dmsg	<'Error - Bad command line switch ... Use ? for help ...'>
	Jmp	Err_Exit
Err_Exit:
	Sub	Ah,Ah
	Or	Al,Al
	Jnz	@F
	Or	Al,1
@@:	Jmp	Pgm_Exit
;+++++++++++++++++++++++++++++++++++++++|
;	Get CURRENT WORK DIRECTORY
;	--------------------------
	IF	OS2
;=======================================|
Get_Cwd:
	MOV	WORD PTR DEFSDIR,0	; Set to NUL
	MOV	WORD PTR [DefSDir_Len],256
	PUSH	DRVNO			; DRIVE NUMBER
	PUSH	DS
	PUSH	OFFSET DEFSDIR		; Put directory here
	PUSH	DS
	PUSH	OFFSET DefSDir_Len	; Length of buffer
	CALL	DOSQCURDIR
	OR	AX,AX
	Mov	Di,Offset DefSDir
	Mov	Dx,Di
	Mov	Cx,Di
	Not	Cx
	Xor	Al,Al
	cld
	RepNe	ScasB
	Dec	Di
	mov	ax,0
	Mov	[Di],Ax
	Mov	Cx,Di
	Sub	Cx,Dx			; Length so far
	Mov	[DefSDir_Len],Cx
	RET
Get_C_Disk:
	PUSH	DS
	PUSH	OFFSET DRVNO
	PUSH	DS
	PUSH	OFFSET DRVMAP
	CALL	DOSQCURDISK		; GET CURRENT DISK
	RET
;=======================================|
	ELSE				; ~OS2 = DOS
;=======================================|
SEEK_ZERO	MACRO
	LOCAL	LSZ1
	MOV	AL,0
LSZ1:	SCASB
	JNE	LSZ1
		ENDM
Get_Cwd:
	PUSH	BX
	PUSH	CX
	PUSH	DX
	PUSH	SI
	PUSH	DI
	PUSH	ES
	PUSH	DS
	POP	ES
	MOV	DX,DRVNO			; DRIVE NUMBER
	MOV	SI,OFFSET DEFSDIR
	MOV	WORD PTR [SI],0			; Start with NUL
	DOS_SUBDIR
	JNC	GET_CWD_2
	JMP	GET_CWD_RET
GET_CWD_2:
	MOV	DX,SI
	MOV	DI,SI
	SEEK_ZERO
	DEC	DI
	MOV	CX,DI
	SUB	CX,DX
	MOV	[DefSDir_Len],CX		; Set the LENGTH
	XOR	AX,AX
GET_CWD_RET:
	OR	AX,AX
	POP	ES
	POP	DI
	POP	SI
	POP	DX
	POP	CX
	POP	BX
	RET
Get_C_Disk:
	DOS_GETDR		; GET CURRENT DISK
	XOR	AH,AH
	INC	AX
	MOV	[DRVNO],AX	; SET DRIVE NUMBER A=1 B=2 C=3 ...
	RET
;=======================================|
	ENDIF				; OS2 Y/N
;;;NoDat	equ	1	; Features
;;;Add_FF	equ	1	;
;;;Include	dos2os2.inc	; Conversion of DOS function (Dos or Os/2)
;;;Include	hexify.inc	; Convert AX into HEX at DS:DI ++++
;;;Include	deci2.inc	; Convert AX or AX:DX to a DECIMAL at ES:DI +10+
BIOS_16:	int	16h
	ret
	NOP
	NOP
	NOP
;=======================================================================
.STACK	STKSIZE
;=======================================================================
	END	BEGIN_TEST
   end ASM */

// eof - cwd32.cpp
