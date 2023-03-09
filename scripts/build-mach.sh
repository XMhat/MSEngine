#!/bin/sh

# This file is part of the MS-Engine source repository.
# @ https://github.com/XMhat/MSEngine
# Copyright (c) MS-Design, 2006-present. All Rights Reserved.

printf '\33c\e[3J'

cd ~/Assets/MSEngine

# g++ -DALPHA -g -O0 -target arm64-apple-macos12 -I./src -I./include -I./include/ft -std=c++20 -stdlib=libc++ -framework AudioUnit -framework CoreAudio -framework AudioToolbox -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL ./lib/ssl64.ma ./lib/glfw64.ma ./lib/lzma64.ma ./lib/zlib64.ma -lcurses -o ./bin/build.mac ./src/build.cpp
# g++ -O3 -DBETA -m64 -arch x86_64 -I./src -I./include -I./include/ft -std=c++20 -stdlib=libc++ -framework AudioUnit -framework CoreAudio -framework AudioToolbox -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL ./lib/ssl64.ma ./lib/glfw64.ma ./lib/lzma64.ma ./lib/zlib64.ma -lcurses -o ./bin/build.mac ./src/build.cpp
g++ -O3 -DBETA -target arm64-apple-macos12 -I./src -I./include -I./include/ft -std=gnu++20 -stdlib=libc++ -framework AudioUnit -framework CoreAudio -framework AudioToolbox -framework Cocoa -framework IOKit -framework CoreVideo -framework OpenGL ./lib/ssl64.ma ./lib/glfw64.ma ./lib/lzma64.ma ./lib/zlib64.ma -lcurses -o ./bin/build.mac ./src/build.cpp

#    $(CC) main.c -o x86_app -target x86_64-apple-macos10.12
#    $(CC) main.c -o arm_app -target arm64-apple-macos11
#   lipo -create -output universal_app x86_app arm_app
