# Source code

This is the source code to MS-Engine which you need to use a compatible C++ compiler in order to compile. The `setup.hpp` module will try to detect and setup a compatible environment automatically so you do not need any special defines or flags except for specifying `ALPHA`, `BETA` or `RELEASE`.

## Guidelines

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

## Copyright © 2006-2023 MS-Design. All Rights Reserved.
