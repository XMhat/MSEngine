# MS-Engine

## Contents…
1. [About](#about)…
2. [Why?](#why)…
3. [Features](#features)…
4. [Using](#using)…
5. [Scripts](#scripts)…
6. [Examples](#examples)…
7. [Credits](#credits)…
8. [Contributing](#contributing)…
9. [Licence and disclaimer](#licence-and-disclaimer)…

## About…
MS-Engine (*Mhat'S Engine*) attempts to be a safe, simple and fast [cross-platform](https://en.wikipedia.org/wiki/Cross-platform_software) [2-D](https://en.wikipedia.org/wiki/2D_computer_graphics) [multimedia](https://en.wikipedia.org/wiki/Multimedia) [engine](https://en.wikipedia.org/wiki/Game_engine) written in [C++20](https://en.wikipedia.org/wiki/C%2B%2B20) for [Visual C++](https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B), [C-Lang](https://en.wikipedia.org/wiki/Clang) and [GNU C++](https://en.wikipedia.org/wiki/GNU_Compiler_Collection) Compilers. MS-Engine brings together many [open-source](https://en.wikipedia.org/wiki/Open_source) [libraries](https://en.wikipedia.org/wiki/Library_(computing)) into one easy-to-use environment controlled by the [LUA interpreter](https://en.wikipedia.org/wiki/Lua_(programming_language)). Right now MS-Engine aims to operate on [Windows XP](https://en.wikipedia.org/wiki/Windows_XP) and [better](https://en.wikipedia.org/wiki/List_of_Microsoft_Windows_versions) (x86 plus [x86-64](https://en.wikipedia.org/wiki/Windows_XP_Professional_x64_Edition)), [MacOS](https://en.wikipedia.org/wiki/MacOS) ([x86-64](https://en.wikipedia.org/wiki/OS_X_Mountain_Lion) plus [Arm64](https://en.wikipedia.org/wiki/MacOS_Big_Sur)) and [Linux](https://en.wikipedia.org/wiki/Linux) ([Ubuntu](https://en.wikipedia.org/wiki/Ubuntu) x86-64).

## Why?…
There are so many multimedia engines out there and most of them are probably better than mine, but I wanted to make one of my own which can be completely controlled by the easy to use LUA language that was introduced to me thanks to [World of Warcraft](https://www.wowhead.com/guide/comprehensive-beginners-guide-for-wow-addon-coding-in-lua-5338). I also wanted a compact engine where all the files could be condensed inside the executable using the tightest compression. Alas, this is difficult on MacOS with their `.app` bundles but I try my best to make those compact too. I would consider myself an intermediate programmer so I am not perfect and would welcome improvements to code using my coding style.

You might wonder why I decided to write this software using a 3-D sound and rendering API? The reason is because these API's seemed more simple to use for me than SDL at the time I was writing this for Windows in 2006 and I was able to get them to compile just fine the way I wanted. I remember I originally had problems with compiling SDL statically on Windows which also seemed kind of bloated as well compared to the 3-D API's. Nowadays, it may be more feasible to use SDL as the issues I was experiencing seem to be resolved now but that would be a major effort to do now. Nevertheless, I do think that I would possibly like to someday extend the operability to a 3-D engine too which is not far from difficult since the systems to do so are already in place!

## Features…
* Scaleable to limits of your hardware and operating system.
* Good portability and cross-platform.
* Use of LUA interpreter with configurability and infinite-loop timeout.
* Full and safe error reporting via extensive use of C++ exceptions.
* Front-end choice of Unix NCurses, Win32 console or OpenGL 2-D interface.
* Asynchronous image format loading (such as PNG, JPG, GIF and DDS).
* Support for transparent framebuffers and windows.
* Optional global 8-bit colour palette system for retro applications.
* Asynchronous audio format loading (such as WAV, CAF, MP3 and OGG).
* Full support for Theora (OGV) video with basic keying using GLSL.
* OpenAL audio with samples, streams and sources interfaces.
* Asynchronous basic HTTP/HTTPS client and SSL socket functionality.
* Optional encryption of your non-volatile data with cvars system.
* Safe directory, memory and file manipulation functions.
* SqLite database support optimised for speed.
* (De)Compression supporting ZIP, LZMA, RAW and AES encryption.
* Very fast JSon support via RapidJson.
* Better optimised binaries from *semi-amalgamated* source code.
* Less than half-a-second startup time to usable on a modern device.

## Using…
All MS-Engine needs to run is an `app.cfg` file in the directory, a 7-zip archive (ending in `.adb` not `.7z`) in the directory or a `.7z` archive appended to the engine executable. This text file contains a list of CVars that configure the engine the way you want. It can be formatted as Windows, MacOS or Unix so it does not matter about the line ending format, but it does have to be UTF-8 (No BoM) formatted which is what all files, filenames and all strings are handled as in the engine. On Windows, UTF-16 filenames are automatically converted to UTF-8.

You also need to set the `app_cflags` cvar properly in the `app.cfg` to specify which subsystems you want to enable. The default value is `0` which means show an error to say you did not set a front-end value. The possible combination of flags you can specify are as follows…

| Flag | Front-end | Purpose | Namespaces |
| --- | --- | --- | --- |
| `0x1` | Yes | Enable Win32 or NCurses Terminal. |
| `0x2` | No | Enable OpenAL subsystem. | `Audio`, `Source`, `Sample` and `Stream`. |
| `0x4` | Yes | Enable OpenGL subsystem. | `Clip`, `Cursor`, `Display`, `Fbo`, `Font`, `Input`, `Palette` and `Texture`. |

Combining certain flags unlocks additional namespaces…

| Flag | Purpose | Namespaces |
| --- | --- | --- |
| `0x6` (`0x2+0x4`) | Enable OpenAL and OpenGL subsystems. | `Video`. |

## Scripts…
The engine looks for the specified file named via the `lua_script` variable which is `main.lua` by default, compiles it and then executes it and waits for a termination request either via a console command, operating system or the script itself and will by default execute continuously based on the `app_tickrate` variable. By default, this is every 1/60th of a second (16.6msec) regardless of hardware limitations.

When the scripts are compiled, the resulting function is cached inside the `<exename>.udb` SqLite database where the engine executable file is or in a writable directory in the user profile so that the script is loaded much faster the next time. Any modification to the text script file will trigger a recompile.

### Writable directory locations…
| System | Location |
| --- | --- |
| Windows | `C:\Users\<UserName>\AppData\Roaming\<AuthorName>\<AppShortName>` |
| MacOS | `/Users/<UserName>/Library/Application Support/<AuthorName>/<AppShortName>` |
| Linux | `/home/<UserName>/.local/<AuthorName>/<AppShortName>` |

MS-Engine does not allow the use of `..` (*parent*) nor the use of a root directory (*Unix*) or drive (*Windows*) prefix for the accessing of any out of scope assets to maintain a sandbox-like safe environment. The only exception is the start-up database name and the working directory which can only be set on the command-line. All backslashes (`\`) in pathnames are automatically replaced with forward-slashes (`/`) for cross-platform compatibility as MSVC's standard C library file functions support unix names natively.

See this [automatically generated document](https://xmhat.github.io/MSEngine) for a complete rundown of all configuration variables and scripting functions. LUA core reference manual is [here](https://www.lua.org/manual/5.4/).

## Examples…
A remake of the classic Amiga and DOS game [Diggers](diggers) was made with this engine and available to play. You can open up the executable (Win32 or Linux) or `.adb` archive (MacOS) with `7-Zip`.

## Credits…
This engine makes use of the following open-source and commercially distributable components that are always updated to the latest versions...

| Library | Version | Author | Purpose |
| --- | --- | --- | --- |
| [7-Zip](https://7-zip.org/sdk.html) | 22 | Igor Pavlov | Powerful general data codec. |
| [FreeType](https://github.com/freetype/freetype) | 2.13 | © The FreeType Project | Load and render TTF fonts. |
| [GLFW](https://github.com/glfw/glfw) | 3.3 | © Marcus Geelnard & Camilla Löwy | Interface to Window, OpenGL and input. |
| [LibJPEGTurbo](https://github.com/libjpeg-turbo/libjpeg-turbo) | 3.0 | © IJG/Contributing authors | Codec/container for YUV pixel data. |
| [LibNSGif](https://github.com/netsurf-browser/libnsgif) | 0.2 | © Richard Wilson & Sean Fox | Codec/container for motion paletted RGB pixel data. |
| [LibPNG](https://github.com/glennrp/libpng) | 1.6 | © Contributing authors | Codec/container for RGB pixel data. |
| [LUA](https://github.com/lua/lua) | 5.4 | © Lua.org, PUC-Rio | User command interpreter. |
| [MiniMP3](https://www.pschatzmann.ch/home/2022/05/14/the-minimp3-codec-on-an-esp32/) | 1.0 | Martin Fiedler | Codec/Container for PCM audio data. |
| [NCurses](https://linux.die.net/man/3/ncurses) | 6.4 | © Free Software Foundation | Unix and MacOS text mode support. |
| [Ogg](https://github.com/xiph/ogg) | 1.3 | Xiph.Org | Container for Vorbis and Theora data. |
| [OpenALSoft](https://github.com/kcat/openal-soft) | 1.23 | Chris Robinson | 3-D audio API. |
| [OpenSSL](https://github.com/openssl/openssl) | 3.1 | OpenSSL Software Foundation | Basic SSL networking and (de/en)cryption. |
| [RapidJson](https://github.com/Tencent/rapidjson) | 1.1 | © THL A29 Ltd., Tencent co. & Milo Yip | Store and access JSON objects. |
| [SQLite](https://github.com/sqlite/sqlite) | 3.41 | Contributing authors | Store and access user non-volatile data. |
| [Theora](https://github.com/xiph/theora) | 3.2 | Xiph.Org | Codec for motion YUV and PCM audio data. |
| [Vorbis](https://github.com/xiph/vorbis) | 1.3 | Xiph.Org | Codec for PCM audio data. |
| [Z-Lib](https://github.com/madler/zlib) | 1.3 | © Jean-loup Gailly & Mark Adler | Common general data codec. |

## Contributing…
Please read [this document](contributing.md) for information on how to contribute and the people who have helped contribute to the project.

## Licence and disclaimer…
Please read [this document](licence.md) for the licence and disclaimer for use of this software.

### [Back to contents](#contents)…

## Copyright © 2006-2023 MS-Design. All Rights Reserved.
