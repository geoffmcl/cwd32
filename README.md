# cwd32 project

## Outline

Windows console app, originally creaded by AppWizard. Presently Windows ONLY, as no effort has been put into porting it to unix.

This file contains a summary of what you will find in each of the files that
make up your cwd32 application.

src\cwd32.cpp  
    This is the main application source file.

src\cwd32.h  
    This is the main application header
    
src\cwd32Vers.h  
	Where the version history is kept

build\build-me.bat
    A generic build batch file using cmake
    
## Building

Uses CMake to generate the build files of your choice.

```
  cd build  # do not build in the root
  cmake -S .. [options] [-A Win32|x64]
  cmake --build . --config Release
```

The is a build-me.bat file, which can maybe be modified to suit your environment...

## License

   * GNU GPL version 2
   * Copyright (c) 1089 ... 2020 - Geoff R. McLane
   
## Usage

Essentially I use it to create PC navigation batch files.

A cmd of `cwd32 "-cst cwd32 src bld" c:\mdos\hcwd.bat` will produce -

```
@D:
@CD \UTILS\cwd32\build
@call st cwd32 src bld
```

Since `C:\MDOS` is in my `PATH` environment variable, I can, from anywhere, type `hcwd` and be back in my cwd32 build directory...

Use -?, --help, or any invalid option, to view the brief help.

```
Informatique Rouge *** CURRENT WORK DIRECTORY UTILITY *** 7 June, 2020 (MSVC16.x64)
 Purpose : Writes the current Drive and Directory to output
 Usage   : CWD32 [Options] [FileName]
 Options: Each preceded by / or - and space separated.
 -@[-|+]   Add an @ character to commands. Default is yes. -@- to not...
 -a        Append to any existing file, if any.
 -b        Bare CWD, single line.
 -cName    Add 'Call Name' to output (Can be repeated).
 -o        Overwrite any existing output file.
 -sName    Add 'Set PROJ=Name' to output.
 -v[0|9]   Silent or very verbal.
 -l        Single line output.
 -8        Output DOS 8.3 format.
 Notes   : A -?, --help, or any invalid option, will produce this help.
 Compiled on Jun  8 2020, at 19:28:47
ERROR: Invalid command switch! [--help]?
```

Note, this app was started sometime before 1994, in Intel assembler, and later re-written in C++.

Have FUN... Geoff.

; eof
   