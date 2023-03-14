# Diggers Fan Remake for MacOS, Linux and Windows
Please see this [readme.txt](/diggers/readme.txt?raw=true) file for more information.

# Screenshots
![Configuration screen](/docs/diggers/ss01.png?raw=true "Configuration screen")
![Title screen](/docs/diggers/ss02.png?raw=true "Title screen")
![Controller screen](/docs/diggers/ss03.png?raw=true "Controller screen")
![Intro video](/docs/diggers/ss04.png?raw=true "Intro video")
![Lobby screen](/docs/diggers/ss05.png?raw=true "Lobby screen")
![Race select screen](/docs/diggers/ss06.png?raw=true "Race select screen")
![Controller screen 2](/docs/diggers/ss07.png?raw=true "Controller screen 2")
![Load/Save screen](/docs/diggers/ss08.png?raw=true "Load/Save screen")
![Book intro screen](/docs/diggers/ss09.png?raw=true "Book intro screen")
![Book screen](/docs/diggers/ss10.png?raw=true "Book screen")
![Map select screen](/docs/diggers/ss11.png?raw=true "Map select screen")
![Level start screen](/docs/diggers/ss12.png?raw=true "Level start screen")
![Level objective screen](/docs/diggers/ss13.png?raw=true "Level objective screen")
![Game screen](/docs/diggers/ss14.png?raw=true "Game screen")
![Game menu actions screen](/docs/diggers/ss15.png?raw=true "Game menu actions screen")
![Game digger inventory screen](/docs/diggers/ss16.png?raw=true "Game digger inventory screen")
![Game diggers position screen](/docs/diggers/ss17.png?raw=true "Game diggers position screen")
![Game status screen](/docs/diggers/ss18.png?raw=true "Game status screen")
![In-game book screen](/docs/diggers/ss19.png?raw=true "In-game book screen")
![In-game lobby screen](/docs/diggers/ss20.png?raw=true "In-game lobby screen")
![In-game shop screen](/docs/diggers/ss21.png?raw=true "In-game shop screen")
![In-game bank screen](/docs/diggers/ss22.png?raw=true "In-game bank screen")
![In-game game over screen](/docs/diggers/ss23.png?raw=true "In-game game over screen")

## Download…
You can always get the latest version of this project [here](https://github.com/XMhat/MSEngine/releases).

## Running…
### Windows version…
Running the Windows version should be trivial.
### Linux version…
The Linux version is linked against external packages that you need to install for the game to work. These packages can be installed with `sudo apt-get install libfreetype libglfw3 libjpegturbo0 libncurses libogg libopenal libpng libsqlite3 libssl libtheora libvorbis libxrandr mesa-common zlib1g`. You can read about the libraries in the [readme.md](readme.md) file.
### MacOS version…
The MacOS version is self-contained and only basic stdlibs are required to run which should already be included by default in any system. However, opening the disk image will require you to bypass Gatekeeper as I do not wish to sign it with personal information and have therefore used a self-signed certificate instead. To bypass Gatekeeper, simply run the `Terminal.app` and `chdir` to the directory where you downloaded the disk image with `chdir ~/Downloads` and then execute the following command to remove the quarantine flag from the disk image archive with `xattr -d com.apple.quarantine Diggers-R<x>-MacOS-Universal.dmg` where `<x>` is the version of this application. You should then be able to mount the disk image and run the game without any issue.

## Copyright © 2006-2023 MS-Design. All Rights Reserved.
