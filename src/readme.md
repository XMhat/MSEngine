# Source code

This is the source code to MS-Engine which you need to use a compatible C++ compiler in order to compile.

## Contents…
* [Compile](#compile)…
  * [MacOS](#macos)…
  * [Windows](#windows)…
  * [Linux](#linux)…
  * [Defines](#defines)…
* [Guidelines](#guidelines)…

## Compile…
I use my own project management system binary to build MS-Engine so there are no build scripts provided. The `setup.hpp` module will try to detect and setup a compatible environment automatically. Below are some hints on how to compile it yourself though by the command-line…

### MacOS…
* Arm64 (Big Sur): `g++ -c -stdlib=libc++ -Wextra -Wall -std=c++20 -Iinclude -Isrc -Iinclude/ft -mmacosx-versionmin=11.0 -arch arm64 -mtune=apple-m1 -DBETA -O2 -odebug/msengine-arm64.o src/msengine.cpp`
* `ld -demangle -syslibroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform Developer/SDKs/MacOSX.sdk -arch arm64 -platform_version macos 11.0 11.0 -o bin/msengine64.mac debug/msengine-arm64.o <libs> -lc -lc++ -lSystem -framework AudioUnit -framework AudioToolbox -framework Cocoa -framework CoreAudio -framework CoreVideo -framework IOKit -framework OpenGL`
* X86-64 (Lion): `g++ -c -stdlib=libc++ -Wextra -Wall -std=c++20 -Iinclude -Isrc -Iinclude/ft -mmacosx-version-min=10.7 -arch x86_64 -mtune=generic -DBETA -O2 -odebug/msengine-x86-64.o src/msengine.cpp`
* `ld -demangle -syslibroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk -arch x86_64 -platform_version macos 10.7 10.7 -lcrt1.10.6.o -o bin/msengine32.mac debug/msengine-x86-64.o <libs> -lc -lc++ -lSystem -framework AudioUnit -framework AudioToolbox -framework Cocoa -framework CoreAudio -framework CoreVideo -framework IOKit -framework OpenGL`

### Windows…
* X86 (XP+): `CL.EXE -nologo -c -MP4 -GA -Gy -GF -EHsc -bigobj -std:c++20 -utf-8 -W4 -I<idirs> -DBETA -MT -Z7 -O2 -GS- -GR- -Gw -Qpar -Fodebug/msengine32.obj src/msengine.cpp`
* `LINK.EXE -nologo -wx -machine:x86 -largeaddressaware -subsystem:windows,5.01 -out:bin/msengine32.exe -pdb:debug/msengine32.pdb -debug -fixed -opt:ref,icf debug/msengine32.obj <libs> kernel32.lib user32.lib gdi32.lib ole32.lib version.lib imagehlp.lib dbghelp.lib advapi32.lib psapi.lib winmm.lib comctl32.lib shell32.lib ws2_32.lib crypt32.lib debug/msengine.res`
* X86-64 (XP+): `CL.EXE -nologo -c -MP4 -GA -Gy -GF -EHsc -bigobj -std:c++20 -utf-8 -W4 -I<idirs> -DBETA -MT -Z7 -O2 -GS- -GR- -Gw -Qpar -Fodebug/msengine-x86-64.obj src/msengine.cpp`
* `LINK.EXE -nologo -wx -machine:x64 -subsystem:windows,5.02 -out:bin/msengine64.exe -pdb:debug/msengine64.pdb -debug -fixed -opt:ref,icf debug/msengine64.obj <libs> kernel32.lib user32.lib gdi32.lib ole32.lib version.lib imagehlp.lib dbghelp.lib advapi32.lib psapi.lib winmm.lib comctl32.lib shell32.lib ws2_32.lib crypt32.lib debug/msengine.res`

### Linux…
* X86-64 (Ubuntu 22.04): `g++ -c -shared-libgcc -mtune=generic -fmax-errors=1 -funwind-tables -Wextra -std=c++20 -I<idirs> -march=x86-64 -DBETA -O2 -odebug/msengine64.o src/msengine.cpp`
* `/usr/lib/gcc/x86_64-linux-gnu/11/collect2 -plugin /usr/lib/gcc/x86_64-linux-gnu/11/liblto_plugin.so -plugin-opt=/usr/lib/gcc/x86_64-linux-gnu/11/lto-wrapper -plugin-opt=-pass-through=-lgcc_s -plugin-opt=-pass-through=-lgcc -plugin-opt=-pass-through=-lc --build-id --eh-frame-hdr --hash-style=gnu --as-needed -pie -z now -z relro /usr/lib/gcc/x86_64-linux-gnu/11/../../../x86_64-linux-gnu/Scrt1.o /usr/lib/gcc/x86_64-linux-gnu/11/../../../x86_64-linux-gnu/crti.o /usr/lib/gcc/x86_64-linux-gnu/11/crtbeginS.o -L/usr/lib/gcc/x86_64-linux-gnu/11/ -dynamic-linker /lib64/ld-linux-x86-64.so.2 -m elf_x86_64 /usr/lib/gcc/x86_64-linux-gnu/11/crtendS.o /usr/lib/gcc/x86_64-linux-gnu/11/../../../x86_64-linux-gnu/crtn.o -o bin/msengine64.elf --export-dynamic debug/msengine64.o -l<libs> -lsqlite3 -ljpeg -lz -lvorbis -lvorbisfile -logg -ltheora -ltheoradec -lfreetype -lglfw -lpng -lcrypt -lssl -lopenal -lrt -ldl -lX11 -lncursesw -lpthread -lstdc++ -lm -lgcc_s -lgcc -lc -lgcc_s -lgcc`

* Where `<idirs>` are where the include files of `OpenAL`, `FreeType`, `GLFW`, `LibJPEGTurbo`, `Lua`, `LZMA`, `MiniMP3`, `NCurses`, `LIBNSGif`, `LibOgg`, `LibPng`, `LibSqlite`, `OpenSSL`, `Theora` and `Z-Lib` are. I made them all relative so I can easily modify them if I need to and automatically have them backed up.
* Where `<libs>` are the lib files of `OpenAL`, `FreeType`, `GLFW`, `LibJPEGTurbo`, `Lua`, `LZMA`, `MiniMP3`, `NCurses`, `LIBNSGif`, `LibOgg`, `LibPng`, `LibSqlite`, `OpenSSL`, `Theora` and `Z-Lib`. I made these all relative paths too so I can custom compile them and have them automatically backed up.

### Notes…
On Linux, some built-in libs (via `apt-get`) are used and I only made my own compilations of `LibJPEGTurbo`, `Lua`, `LZMA`, `MiniMP3`, `LibNSGif` and `OpenSSL` instead of the ones in the previous paragraphs. I also directly copied the link command from output logs. I only compiled and tested this with `Ubuntu 22.04 LTS` on real hardware unfortunately, so it might not work on your distribution and I don't intend to support anything else.

### Defines…
Below are the only options you can define to modify engine behaviour…

| Define | Purpose |
| --- | --- |
| `ALPHA` | Use when enabling debugging and exposes memory previews with the `assets` command. |
| `BETA` | For normal development uses with all features and no restrictions. |
| `RELEASE` | Disables the console by default, hardens checks and hides certain functionality that the end-user does not need. |

## Guidelines…
* Text file formatting…
  * No lines longer than *80* characters for text mode compatibility so hard-wrap long lines to next indentation.
  * Always use `UTF-8` with *no BoM*.
  * *Unix* line-feeds only and no carriage returns.
  * *Two* whitespace characters per indentation.
  * No tab characters.
  * No empty lines wasting screen estate.
* File names…
  * Do not ever use `.cpp`, `.c` modules or `.h` header files unless they are from a third-party API. The only `.cpp` file needed is `msengine.cpp` and all other code is stored in `.hpp` files.
  * Keep filenames short (`ll` prefix is `LuaLib` code, and `sys` prefix is `System` code, `pix` prefix is `posix`, `win` prefix is `Windows`).
* Code style…
  * Curly braces on a new line usually except when already in function scope.
  * Optimisation over readability, use comments to help with readability.
  * Prefix all variable names with the typename (e.g. `char*` = `cpVar`, `int` = `iVar`, `MyClassName` = `mcnVar`).
  * All function names and class/struct typenames begin with a uppercase character.
  * All macro names are uppercase.
  * Comment every line unless a single comment can describe two or more lines.
  * No prototyping unless *absolutely* necessary.
  * `#define` functions are discouraged so use C++ unless absolutely neccesary.
  * Keep `#macro` statements indented *after* the hash (`#`) symbol and separate from the indentation of the interpreted code.
  * Limit use of C code unless communicating with third-party API's.
  * Keep code as compact as possible and try to eliminate duplicate code.
  * Keep code categorised in namespaces and structs and out of global scope.
  * Always use the latest `CppCheck` to verify code.
* Operating system specific…
  * Writing operating system specific code means you have to add code for the other operating systems too.
  * Try to always use `posix` functions to keep compatibility with both `MacOS` and `Linux`.
* Compiling and linking…
  * Must compile with no warnings with `-Wall -Wextra` on MacOS C-Lang, `-Wextra` on Linux GCC and `/W4` on Windows MSVC.
  * Only use static libraries so we can keep everything in the binary and self-reliant.

Feel free to create an [issue](https://github.com/XMhat/MSEngine/issues) or [pull request](https://github.com/XMhat/MSEngine/pulls) to correct issues or use the [discussions](https://github.com/XMhat/MSEngine/discussions) forum for questions.

### [Back to contents](#contents)…

## Copyright © 2006-2023 MS-Design. All Rights Reserved.
