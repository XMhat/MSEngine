![Diggers logo](/docs/diggers/logo.png?raw=true "Diggers logo")

# Diggers Fan Remake for MacOS, Linux and Windows

## Table of contents…
1. [Screenshots](#screenshots)…
2. [System Requirements](#system-requirements)…
3. [Downloading](#downloading)…
4. [Running](#running)…
5. [About](#about)…
6. [Story](#story)…
7. [Objectives](#objectives)…
8. [Setup](#setup)…
9. [Controls](#controls)…
10. [Advanced](#advanced)…
11. [FAQ](#faq)…
12. [Credits](#credits)…
13. [Disclaimer](#disclaimer)…
14. [Licence](#licence)…

## [Screenshots](#screenshots)…
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

## [System Requirements](#system-requirements)…
| | Minimum | Recommended | Note |
| --- | --- | --- | --- |
| **Processor** | Dual-Core 2GHz  | Multi-Core 2GHz+ | Multithreaded! |
| **System** *(Windows)* | XP SP3 *(X86/X64)* | 7/8.X/1X+ *(X64)* | 32 and 64bit executable. |
| **Memory** *(Windows)* | 16MB *(XP/X86)* | 128MB+ *(Win7/X64)* | Close estimate. |
| **System** *(MacOS)* | 10.7 *(Intel)* / 11 *(Arm)* | 13.0+ *(Arm)* | FAT Universal executable. |
| **Memory** *(MacOS)* | 256MB *(Intel)* | 512MB *(Arm)* | Close estimate. |
| **System** *(Linux)* | Ubuntu 22.04 *(X64)* | Ubuntu 22.04+ *(X64)* | See below for packages. |
| **Memory** *(Linux)* | 16MB | 128MB+ | Wild estimate. |
| **Graphics** | NV GF8K/ATI R600 | NV GF8K+/ATI R600+ | OpenGL 3.2 compatible. |
| **Audio** | Any sound device | Any sound device | OpenAL 1.1 compatible. |
| **Input** | Keyboard and mouse | Keyboard and mouse | Supports joystick! |
| **Disk Space** | 48MB (Read-Only) | 64MB (Read+Write) | Portable! |
| **Network** | None | None | Unused (for now!). |

## [Downloading](#downloading)…
You can always get the latest version of this project [here](https://github.com/XMhat/MSEngine/releases).

## [Running](#running)…
### Windows version…
Running the Windows version should be trivial.

### Linux version…
The Linux version is linked against external packages that you need to install for the game to work. These packages can be installed with `sudo apt-get install libfreetype libglfw3 libjpegturbo0 libncurses libogg libopenal libpng libsqlite3 libssl libtheora libvorbis libxrandr mesa-common zlib1g`.

### MacOS version…
The MacOS version is self-contained and only basic stdlibs are required to run which should already be included by default in any system. However, opening the disk image will require you to bypass Gatekeeper as I do not wish to sign it with personal information and have therefore used a self-signed certificate instead. To bypass Gatekeeper, simply run the `Terminal.app` and `chdir` to the directory where you downloaded the disk image with `chdir ~/Downloads` and then execute the following command to remove the quarantine flag from the disk image archive with `xattr -d com.apple.quarantine Diggers-R<x>-MacOS-Universal.dmg` where `<x>` is the version of this application. You should then be able to mount the disk image and run the game without any issue.

## [About](#about)…
Diggers is a puzzle video game for the Amiga CD32 in which the player takes control of a mining team excavating a planet for precious minerals. It was later released for the Amiga 1200 and DOS. Diggers was bundled with the CD32 at launch, along with a 2D platformer entitled Oscar on the same CD. A sequel, Diggers 2: Extractors, was released for DOS in 1995.

The game is set on the planet Zarg, where four races are vying for the gems and ores buried there. The four races are: the Habbish, who dig quickly and have high endurance, yet are impatient and prone to wander off; Grablins, who dig the fastest, have good stamina but are weak fighters; Quarriors, who are slow diggers but strong fighters; and the F'Targs, who are mediocre but regain their health quickly. The player chooses one of the four races, then sends out five man teams to mine a region. There are 34 regions in all, with two accessible at the beginning of the game. Each region has an amount of money the player must amass in order to beat it and open up the adjacent territories. Time plays a role, as there is always a computer controlled opposing race in the region, competing with the player to be the first to achieve the monetary goal.

The gameplay is similar in some ways to Lemmings, with the player issuing orders to his five miners, not directly controlling them. The stages themselves are viewed from the side - the mineshafts that extend throughout the levels give the impression of an anthill. The player commands his units to dig and, when needed, perform more specific actions such as picking up gems and fighting enemy diggers. The races have various personalities, and will occasionally do things without the player commanding them to; this can range from good (such as fighting an enemy) to very bad (walking into a deep pool of water and drowning). As the miners dig, rubies, gold, emeralds, and diamonds will appear.

At any time the player can teleport a miner back to the starting point and send him to the 'Zargon Stock Exchange', in order to trade the finds for money. Only three commodities are traded at one time, and the prices for each fluctuate depending on how much of a particular item has recently been sold. Here the player may also visit a shop and buy special mining tools, such as dynamite, automated drillers and bridge components. The miners themselves are expendable (though a monetary loss will be incurred for each lost worker), with a fresh set of five available each time a new level is entered.

## [Story](#story)…
This is no ordinary day on the planet, Zarg. Today is the glorius 412th. The day that each year, signals the commencement of one months frenzied digging. Four races of diggers, are tunneling their way to the Zargon Mineral Trading centre. they each, have an ambition, that requires them to mine as much as the planets mineral wealth, as possible.

Observing the quarrelsome diggers from afar, is a mysterious stranger. Each of the races are hoping that this stranger, will control their mining operations. His expertise, will be vital, in guiding them along the long, dangerous path that leads to their ultimate goal. His first step, is to register, at the Zargon Mineral Trading centre.

As the diggers wait nervously, the stranger heads toward the trading centre. for him, the ultimate test. the greatest challenge of his life lies ahead. the rewards for success, will be wealth unlimited. the results of failure, are unthinkable!

## [Objectives](#objectives)…
Guide one of the four available races to raising 17,500 Zogs by mining valuable minerals across 34 zones in order to build their dream creation.

## [Setup](#setup)…
Press `F1` at any time in the game to access the setup screen and `F2` to show the acknowledgements.

### Configuration screen…
* **Monitor**
  - Allows you to change the monitor the game will be displayed on. This is only useful if you have multiple monitors. Use 'Primary Monitor' if you don't care or have only one monitor.
* **Display State**
  * Allows you to play the game in full-screen or windowed mode.
  * **Windowed mode**
    - Play the game in a resizable/decorated window. You can control the size of this window by the appropriate window size option below.
  * **Borderless Full**
    - Play the game in a full-screen borderless window.
  * **Exclusive Full**
    - Play the game in exclusive full-screen mode. You can change the resolution with the appropriate full-screen size option below.
* **Full-Resolution**
  * Changes the full-screen resolution to play the game in. This option shows 'Disabled' if 'Display State' is not set to 'Exclusive full-screen mode'.
* **Window Size**
  * Changes the size of the window when 'Display State' is set to
  * **Windowed mode**
    - Disabled if not set as such.
  * **Automatic**
    - Sets the size of the window automatically that is appropriate for your desktop resolution.
* **Frame-Limiter**
  * Most games like this one will normally try to render the graphics as fast as possible this results in straining of the GPU with unneccesary screen tearing, power usage and heat. You only need to ever unlock the FPS to benchmarking reasons only as your monitor will only display frames at a fixed rate (normally 60fps [or 60hz]), thus it is advisable to turn this on to ensure longevity of your equipment. There will also be additional strain on the CPU as well as it sends commands to the GPU as fast as possible as well. Options available are...
  * **Adaptive VSync**
    - Turn on vertical synchronization only when the frame rate of the software exceeds the display's refresh rate, disabling it otherwise. That eliminates the stutter that occurs as the rendering engine frame rate drops below the display's refresh rate.
  * **None**
    - Renders as fast as possible.
  * **VSync only**
    - The video card is prevented from doing anything visible to the display memory until after the monitor finishes its current refresh cycle.
  * **Double VSync only**
    - Same as *VSync only* but limits to half the refresh rate.
  * **Adaptive VSync & Suspend**
    - Same as *Adaptive VSync* but adds a one millisecond suspend to the engine thread on the processor.
  * **Suspend Only**
    - Sane as *None* but only adds a one millisecond suspend to the engine thread on the processor.
  * **VSync & Suspend**
    - Same as *VSync only* and adds a one millisecond suspend to the engine thread on the processor.
  * **Double VSync & Suspend**
    - Same as *Double VSync only* and adds a one millisecond suspend to the engine thread on the processor.
* **Texture Filter**
  * Allows you to apply bilinear filtering to the game.
  * **Yes**
    - Bilinear filtering enabled by the GPU.
  * **No**
    - Point filtering (Retro and fastest).
* **Audio Device**
  - Allows you to select which device you want the game sound to play on. Leave it on 'Default' if you don't care. Note that if the device becomes unavailable. The game engine will try to re-initialise it, and if the device does not respond. The default audio device will be used. There may be a few seconds before any sort of detection occurs due to the way OpenAL works.
* **Volumes**
  - Self-explanitory. 100% is full volume (0 dB) and 0% is digital mute. Try not to use high volumes or there will be significant clipping on Windows XP systems, or annoying auto-normalisation of music and sound on Windows 7+ systems. Try to keep the values at three-quarters, i.e. 75%.

### Buttons…
* **APPLY**
  - Save the changes and set the new values.
* **DONE**
  - Return to game without saving or applying any changes.
* **RESET**
  - Resets all the settings to default and applys the default values.
* **ABOUT**
  - Show the acknowledgements (readme.txt).

### About…
This screen allows you to read this document.

* **Basic control…**
  - Move the cursor to the top of the screen with the mouse or joystick and hit the left button or button 1 to scroll the text up.
  - Move the cursor to the bottom of the screen with the mouse or joystick and hit the left button or button 1 to scroll the text up.
  - Press the right mouse button or button 2 to return to the configuration screen.

* **Advanced control…**
  * `ESCAPE KEY`…
    - Return to the game WITHOUT saving or applying settings.
  * `HOME KEY`…
    - Scroll the document to the top.
  * `END KEY`…
    - Scroll the document to the end.
  * `CURSOR UP KEY` or `MOUSE WHEEL UP`…
    - Scroll the document up one line.
  * `CURSOR DOWN KEY` or `MOUSE WHEEL DOWN`…
    - Scroll the document down one line.
  * `PAGE UP KEY` or `MOUSE WHEEL LEFT`…
    - Scroll the document up one page.
  * `PAGE DOWN KEY` or `MOUSE WHEEL RIGHT`…
    - Scroll the document down one page.

## [Controls](#controls)…
You mostly control the game with a mouse, but a joystick will basicly emulate the mouse too like the Amiga CD32 version of the game did. You can also use the keyboard to perform some basic tasks.

If you're using a high video resolution (i.e. 1080 or 1440p) and the mouse appears slow, you may need to increase the sensetivity in your mouse control panel or on specific mice (such as a Logitech G-series), increase the DPI with the dedicated button.

If you move the cursor and the cursor becomes anything but an arrow graphic, then than particular item on the screen is selectable, e.g.
* 4 small arrows pointing to the centre…
  - Select/Perform action/Go here.
* 1 small arrow with `EXIT` underneath…
  - Exit out of particular area.
* 1 small arrow with `OK` underneath…
  - Accept/Select/Perform action/Go here.
* Arrow cursor pointing up/down/left or right…
  - Scroll.

### Aliases…
These aliases are for the explanations below.

| Alias | Meaning | Alias | Meaning |
| --- | --- | --- | --- |
| `LMB` | Left-Mouse Button | `RMB` | Right-Mouse Button |
| `MMB` | Middle-Mouse Button | `MBx` | Mouse Button `x` |
| `MWU` | Mouse Wheel up | `MWD` | Mouse Wheel Down |
| `KPx` | Keypad Button `x` | `JBx` | Joystick Button `x` |
| `JOY` | Joystick | `MSE` | Mouse |
| `KEY` | Keyboard | | |

### Anywhere in the game…
* `Alt`+`F4` on Windows or `Cmd`+`Q` on a Mac…
  - Instant quit. Any game progress is lost but engine settings and ALREADY SAVED game progress will be written to disk in a `.udb` file.
* `Alt`+`Enter`…
  - Switch between Borderless Full-Screen and decorated Window mode.

### Anywhere in the game (but not during a fade transition…
* `F1` or `JB8`+`JB9`…
  - Configure the game engine, video/audio settings, etc.
* `F2`…
  - Display the acknowledgements.
* `F11`…
  - Reset the window position and size.
* `F12`…
  - Take a screenshot (lossless .bmp where .exe file is).
* `JOY` or `MSE` axes…
  - Move cursor on screen.
* `JB1` or `LMB`…
  - Select control under mouse cursor.

### Engine setup screen…
* `LMB` or `JB1`…
  - Scroll readme document at top or bottom of screen.
* `RMB`, `JB2` or `ESCAPE`…
  - Leave readme or setup screen.
* `MWU`, `JB2`, `JB5` or `JB7`…
  - Select next item.
* `MWD`, `JB0`, `JB4` or `JB6`…
  - Select precious item.

### Intro…
* `ESC`, `LMB` or `JB1`…
  - Skip cut-scene/intro.

### In-Level…
* `PAUSE` or `START`…
  - Pause and unpause the game.
* `ESCAPE` or `START`+`JB8`+`JB9`…
  - Forfeit the game only when paused (Game over).
* `1` or `KP1`…
  - Select first digger if alive.
* `2` or `KP2`…
  - Select second digger if alive.
* `3` or `KP3`…
  - Select third digger if alive.
* `4` or `KP4`…
  - Select fourth digger if alive.
* `5` or `KP5`…
  - Select fifth digger if alive.
* `Backspace`…
  - Stop selected digger.
* `LMB` or `JB1`…
  - Action, Select or Cancel menu.
* `RMB` or `JB2`…
  - Open menu for selected digger.
* `RETURN`…
  - Grab item Digger maybe touching.
* `UP CURSOR`…
  - Jump (When digging left/right, press to dig diagonally up).
* `DOWN CURSOR`…
  - Dig down (When digging left/right, press to dig diag. down).
* `LEFT CURSOR`…
  - Move left (press again to run, press a third time to dig).
* `RIGHT CURSOR`…
  - Move right (press again to run, press a third time to dig).
* `MINUS KEY` or `JB8`…
  - Select last Digger.
* `EQUALS KEY` or `JB9`…
  - Select next digger.
* `KP0`…
  - Teleport home/nearest telepole.

### In-shop/Race select screen…
* `MWU` or `MWD`…
  - Scroll available options.
* `LMB`…
  - Select item under cursor.

## [Advanced](#advanced)…
If for some reason you're an advanced-user or admin and need to fine tune how the game engine works, we have some command-line parameters that might be useful and you can overload as much as the operating system allows…

* Syntax: `diggers.exe cvar=value [cvar=value [...]]`

### Examples:-
* `diggers.exe vid_fs=1`
  - Will make the engine startup in full-screen.
* `diggers.exe con_disabled=0`
  - Will enable the developer console.
* `diggers.exe vid_fs=1 con_disabled=0`
  - Will make the engine startup in full-screen and enable the developer console.

You can change cvars by opening up the console with the `GRAVE` key (key under `ESCAPE` key). You will have to start the game with `con_disabled=0` in order for this to work. This key may not be available on most keyboards so you can change it by changing the `con_key1` or `con_key2` var. This is a GLFW specific key code. Once you're in the console, type `cmdlist` for the commands you can use or 'cvarlist' for the settings you can change. In the release version of the game, the console is semi-permanantly disabled, you can re-enable the console by using `con_disabled=0` to re-enable it. CVars can obviously be overloaded onto the command-line, not just one setting.

## [F.A.Q.](#faq)…
* **Q. My game controller (joystick) doesn't work?**
  - Restart the game with your controller connected or set `inp_joystick` to `1` in the console. Note that enabling joystick polling may reduce performance.
* **Q. This does not 'look' like the original game.**
  - Well I've had to make a few changes because I want the remake to support 16:9 widescreen and ive had to modify and adapt most of the old textures that were originally in 320x200 (16:10), which is not a 4:3 resolution. If you can help upgrade the textures, that would be awesome!
* **Q. I got an error, strange behaviour or found a bug.**
  - You can start the game with the `log_file=...` (filename) parameter to give a pretty detailed log of what the app is doing so you can send me that along with as much info as possible such as the `.log` `.crt` `.dbg` and `.udb` files that the app generates. Neither of these files will contain any personal information, only technical information to help me squash the problem.
* **Q. I've picked the wrong game-engine settings and the game crashed/won't start.**
  - Try specifying the parameter `sql_defaults=1` command-line option to reset all the engine settings to default. If this doesn't work you'll have to delete the `.udb` file or set `sql_defaults=2` and start again from scratch thus losing all your Diggers saved game data. All that said though, the game shouldn't really crash though so please send me any logs/crash dumps you may have.

## [Credits](#credits)…
### Diggers for Amiga, CD32 and DOS…
* **Programmed by…**
  - *Mike Froggatt* (PC) and *Toby Simpson* (Amiga).
* **Additional DOS programming…**
  - *Keith Hook*.
* **Original design and Amiga programming…**
  - *Toby Simpson*.
* **Graphics…**
  - *Jason Wilson*.
* **Music and effects…**
  - *Richard Joseph* and *Graham King*.
* **Additional graphics…**
  - *Tony Heap* and *Jason Riley*.
* **Narrative text…**
  - *Martin Oliver*.
* **Additional design…**
  - *Michael Hayward*, *Ian Saunter* and *Tony Fagelman*.
* **Level design…**
  - *Toby Simpson* and *Tony Fagelman*.
* **Quality assurance…**
  - *Steve Murphy*, *Paul Dobson* and *Kelly Thomas*.
* **Intro & outro sequence…**
  - *Mike Ball* and *Mike Froggatt*.
* **Book production…**
  - *Alan Brand*, *Steve Loughran*, *Matrin Oliver* and *Tony Fagelman*.
* **Special thanks…**
  - *Chris Ludwig*, *Wayne Lutz*, *Dave Pocock*, *Sharon McGuffie* and *Ben Simpson*.
* **Produced By…**
  - *Tony Fagelman* and *Millennium*.

### Diggers fan remake for Windows, MacOS and Linux…
* **Complete conversion…**
  - By *Mhat* of *MS-Design*.
* **Setup music…**
  - *Super Stardust - Special Mission 1* by *PowerTrace*.
* **Credits music…**
  - *4U 07:00 V2001* by *Enuo*.
* **Game over music loop…**
  - *1000 Years Of Funk* by *Dimitri D. L.*.
* **Special thanks…**
  - [ModArchive.Org](https://www.modarchive.org).
  - [AmigaRemix.Com](https://www.amigaremix.com).

Please read [this readme document](/readme.md) on the third party software used in this game.

## [Disclaimer](#disclaimer)…
Diggers is *© Millennium Interactive Ltd. 1994* to which the company was [closed](https://www.mobygames.com/company/298/guerrilla-cambridge/) a long time ago. This game is not endorsed, nor supported by any aforementioned person, persons, entity nor entities. Please *do not* attempt to contact anyone aforementioned in this document, nor any author of the third-party credits for support. Any and all queries should *only* be directed to the dedicated [issues](https://github.com/XMhat/MSEngine/issues) and [discussions](https://github.com/XMhat/MSEngine/discussions) pages on GitHub.

## [Licence](#licence)…
Please read [this licence document](/licence.md) for the licence and disclaimer for use of this software.

## Copyright © 2006-2023 MS-Design. All Rights Reserved.
