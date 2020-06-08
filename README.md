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

  cd build  # do not build in the root
  cmake -S .. [options] [-A Win32|x64]
  cmake --build . --config Release
  
The is a build-me.bat file, which can maybe be modified to suit your environment...

## License

   * GNU GPL version 2
   * Copyright (c) 1089 ... 2020 - Geoff R. McLane
   
## Usage

Use -? to view the help.

Essentially I ise it to create PC navigation batch files.

A cmd of `cwd32 "-cst cwd32 src bld" c:\mdos\hcwd.bat"` will produce -

```
@D:
@CD \UTILS\cwd32\build
@call st cwd32 src bld
```

Have FUN... Geoff.

; eof
   