#############################################################################
## Diggers R29   A remake of the Amiga/DOS game for Windows, Linux & MacOS ##
## Created by Toby Simpson & Mike Froggatt (c) Millennium Interactive 1994 ##
## Powered by MS-Engine (X-Platform Multimedia Engine)  (c) MS-Design 2023 ##
## Visit https://github.com/XMhat/Diggers for more information and updates ##
#############################################################################

#############################################################################
## Table of contents                                                    00 ##
#############################################################################
System requirements .................................................... [01]
About Diggers .......................................................... [02]
Story of the game ...................................................... [03]
Objective of the game .................................................. [04]
Setup .................................................................. [05]
Controls ............................................................... [06]
For advanced users ..................................................... [07]
Frequently-asked questions & answers ................................... [08]
Credits ................................................................ [09]
Licence ................................................................ [10]

#############################################################################
## System requirements                                                  01 ##
#############################################################################
            MINIMUM             RECOMMENDED          NOTE
CPU.......: Dual-Core 2GHz      Multi-Core 2GHz+     Multithreaded!
OS(Win32).: XP SP3 (X86/X64)    7/8.X/1X+ (X64)      32 and 64bit EXE
RAM(Win32): 16MB (XP/X86)       128MB+ (Win7/X64)    Close estimate
OS(MacOS).: 10.7(Intel)/11(Arm) 13.0+ (Arm)          FAT Universal EXE
RAM(MacOS): 256MB (Arm)         512MB (Arm)          Close estimate
OS(Linux).: Ubuntu 22.04 (X64)  Ubuntu 22.04+ (X64)  See below for packages
RAM(Linux): 16MB                128MB+               Wild estimate
GFX.......: NV GF8K/ATI R600    NV GF8K+/ATI R600+   OpenGL 3.2 compatible
Audio.....: Any sound device    Any sound device     OpenAL 1.1 compatible
Input.....: Keyboard and mouse  Keyboard and mouse   Supports joystick!
Disk Space: 48MB (Read-Only)    64MB (Read+Write)    Portable!
Network...: None                None                 Unused (for now!)

The Linux version is linked against external packages that you need to
install for the game to work. These packages can be installed with
'sudo apt-get install' and are as follows...

* mesa-common  libxrandr  libjpegturbo0  libncurses   zlib1g  libsqlite3
  libogg       libvorbis  libtheora      libfreetype  libpng  libssl
  libopenal    libglfw3

The MacOS version is self-contained and only basic stdlibs are required to
run which should already be included by default in any system. However,
opening the disk image will require you to bypass Gatekeeper as I do not wish
to sign it with personal information and have therefore used a self-signed
certificate instead. To bypass Gatekeeper, simply run the 'Terminal.app' and
'chdir' to the directory where you downloaded the disk image, for example...

* chdir ~/Downloads

Then execute the following command to remove the quarantine flag from the
disk image archive...

* xattr -d com.apple.quarantine Diggers-R<x>-MacOS-Universal.dmg

Where <x> is the version of this application. You should then be able to
mount the disk image and run the game without any issue.

Disk image layout...
+ Diggers.app                          = Base application bundle directory.
| +- Contents                          = Bundle contents directory.
|    +- Info.plist                     = Bundle settings.
|    +- MacOS                          = Bundle binary.
|    |  +- diggers                     = Main X86-64/ARM64 binary.
|    +- PkgInfo                        = Package information.
|    +- Resources                      = Bundle resources.
|    |  +- Credits.rtf                 = Copy of this readme file.
|    |  +- Icon.icns                   = Bundle application icon file.
|    |  +- diggers.adb                 = Consolidated game data archive.
|    +- _CodeSignature                 = Code signature directory.
|       +- CodeResources               = Self-signed code signature.
+- Drag Diggers.app here to install    = Alias to /Applications directory.
+- Readme.txt                          = Copy of this readme file.

It's possible to consolidate the 'diggers' executable with the 'diggers.adb'
appended to it for easier portability. This is at the user's own descretion
though.

The Windows version of this game is consolidated in a single self-signed and
self-sufficient executable. There should not be any issue running it.

#############################################################################
## About Diggers                                                        02 ##
#############################################################################
Diggers is a puzzle video game for the Amiga CD32 in which the player takes
control of a mining team excavating a planet for precious minerals. It was
later released for the Amiga 1200 and DOS. Diggers was bundled with the CD32
at launch, along with a 2D platformer entitled Oscar on the same CD.
A sequel, Diggers 2: Extractors, was released for DOS in 1995.

The game is set on the planet Zarg, where four races are vying for the gems
and ores buried there. The four races are: the Habbish, who dig quickly and
have high endurance, yet are impatient and prone to wander off; Grablins, who
dig the fastest, have good stamina but are weak fighters; Quarriors, who are
slow diggers but strong fighters; and the F'Targs, who are mediocre but
regain their health quickly. The player chooses one of the four races, then
sends out five man teams to mine a region. There are 34 regions in all, with
two accessible at the beginning of the game. Each region has an amount of
money the player must amass in order to beat it and open up the adjacent
territories. Time plays a role, as there is always a computer controlled
opposing race in the region, competing with the player to be the first to
achieve the monetary goal.

The gameplay is similar in some ways to Lemmings, with the player issuing
orders to his five miners, not directly controlling them. The stages
themselves are viewed from the side - the mineshafts that extend throughout
the levels give the impression of an anthill. The player commands his units
to dig and, when needed, perform more specific actions such as picking up
gems and fighting enemy diggers. The races have various personalities, and
will occasionally do things without the player commanding them to; this can
range from good (such as fighting an enemy) to very bad (walking into a deep
pool of water and drowning). As the miners dig, rubies, gold, emeralds, and
diamonds will appear.

At any time the player can teleport a miner back to the starting point and
send him to the 'Zargon Stock Exchange', in order to trade the finds for
money. Only three commodities are traded at one time, and the prices for each
fluctuate depending on how much of a particular item has recently been sold.
Here the player may also visit a shop and buy special mining tools, such as
dynamite, automated drillers and bridge components. The miners themselves are
expendable (though a monetary loss will be incurred for each lost worker),
with a fresh set of five available each time a new level is entered.

#############################################################################
## Story                                                                03 ##
#############################################################################
This is no ordinary day on the planet, Zarg. Today is the glorius 412th. The
day that each year, signals the commencement of one months frenzied digging.
Four races of diggers, are tunneling their way to the Zargon Mineral Trading
centre. they each, have an ambition, that requires them to mine as much as
the planets mineral wealth, as possible.

Observing the quarrelsome diggers from afar, is a mysterious stranger. Each
of the races are hoping that this stranger, will control their mining
operations. His expertise, will be vital, in guiding them along the long,
dangerous path that leads to their ultimate goal. His first step, is to
register, at the Zargon Mineral Trading centre.

As the diggers wait nervously, the stranger heads toward the trading centre.
for him, the ultimate test. the greatest challenge of his life lies ahead.
the rewards for success, will be wealth unlimited. the results of failure,
are unthinkable!

#############################################################################
## Objective                                                            04 ##
#############################################################################
Guide one of the four available races to raising 17,500 Zogs by mining
valuable minerals across 34 zones in order to build their dream creation.

#############################################################################
## Setup                                                                05 ##
#############################################################################
Press F2 at any time in the game to access the setup screen and F1 to gain
immediate in-game access to this document.

Configuration screen
--------------------
Monitor = Allows you to change the monitor the game will be displayed on.
          This is only useful if you have multiple monitors. Use 'Primary
          Monitor' if you don't care or have only one monitor.

Display State = Allows you to play the game in full-screen or windowed mode.
       |
       +----- Windowed mode = Play the game in a resizable/decorated window.
       |                      You can control the size of this window by
       |                      the appropriate window size option below.
       |
       +----- Borderless Full = Play the game in a full-screen borderless
       |                        window.
       |
       +----- Exclusive Full = Play the game in exclusive full-screen mode.
                               You can change the resolution with the
                               appropriate full-screen size option below.

Full-Resolution = Changes the full-screen resolution to play the game in.
                  This option shows 'Disabled' if 'Display State' is not set
                  to 'Exclusive full-screen mode'.

Window Size = Changes the size of the window when 'Display State' is set to
       |      'Windowed mode'. Disabled if not set as such.
       |
       +----- Automatic = Sets the size of the window automatically that is
                          appropriate for your desktop resolution.

Frame-Limiter = Most games like this one will normally try to render the
       |        graphics as fast as possible this results in straining of the
       |        GPU with unneccesary screen tearing, power usage and heat.
       |        You only need to ever unlock the FPS to benchmarking reasons
       |        only as your monitor will only display frames at a fixed rate
       |        (normally 60fps [or 60hz]), thus it is advisable to turn this
       |        on to ensure longevity of your equipment. There will also be
       |        additional strain on the CPU as well as it sends commands to
       |        the GPU as fast as possible as well. Options available are...
       |
       +------- Disabled = This will render the game as fast as possible.
       |                   Note that if VSync is forced on in your graphics
       |                   driver settings then this option is next to
       |                   useless. This is only useful if you allow
       |                   applications to control the VSync setting and want
       |                   the game to be rendered as fast as possible.
       |
       +------- Hardware = Enable VSync and save GPU power and lower heat.
       |                   This is the recommended option.
       |
       +------- Hardware+Software = Combines both hardware (see above) and
       |                            software based frame limiting
       |                            (see below).
       |
       +------- Software = Suspend the application to save CPU cycles between
                           frames. This may sometimes cause stutter, but
                           saves your GPU and CPU resources for other
                           applications. You can control the delay between
                           frames with the 'vid_delay' cvar.

Texture Filter = Allows you to apply bilinear filtering to the game.
       |
       +------- Yes = Bilinear filtering enabled by the GPU.
       |
       +------- No = Point filtering (Retro and fastest).

Audio Device = Allows you to select which device you want the game sound to
               play on. Leave it on 'Default' if you don't care. Note that if
               the device becomes unavailable. The game engine will try to
               re-initialise it, and if the device does not respond. The
               default audio device will be used. There may be a few seconds
               before any sort of detection occurs due to the way OpenAL
               works.

Volumes = Self-explanitory. 100% is full volume (0 dB) and 0% is digital mute.
          Try not to use high volumes or there will be significant clipping
          on Windows XP systems, or annoying auto-normalisation of music and
          sound on Windows 7+ systems. Try to keep the values at
          three-quarters, i.e. 75%.

APPLY = Save the changes and set the new values.

DONE = Return to game without saving or applying any changes.

RESET = Resets all the settings to default and applys the default values.

HELP = Show this readme document (readme.txt).

Readme document
---------------
This screen allows you to read this document.

Basic control:-
* Move the cursor to the top of the screen with the mouse or joystick and hit
  the left button or button 1 to scroll the text up.
* Move the cursor to the bottom of the screen with the mouse or joystick and
  hit the left button or button 1 to scroll the text up.
* Press the right mouse button or button 2 to return to the configuration
  screen.

Advanced control:-
* ESCAPE KEY        = Return to the game WITHOUT saving or applying settings.
* HOME KEY          = Scroll the document to the top.
* END KEY           = Scroll the document to the end.
* CURSOR UP KEY
  MOUSE WHEEL UP    = Scroll the document up one line.
* CURSOR DOWN KEY
  MOUSE WHEEL DOWN  = Scroll the document down one line.
* PAGE UP KEY
  MOUSE WHEEL LEFT  = Scroll the document up one page.
* PAGE DOWN KEY
  MOUSE WHEEL RIGHT = Scroll the document down one page.

#############################################################################
## Controls                                                             06 ##
#############################################################################
You mostly control the game with a mouse, but a joystick will basicly emulate
the mouse too like the Amiga CD32 version of the game did. You can also use
the keyboard to perform some basic tasks.

If you're using a high video resolution (i.e. 1080 or 1440p) and the mouse
appears slow, you may need to increase the sensetivity in your mouse control
panel or on specific mice (such as a Logitech G-series), increase the DPI
with the dedicated button.

If you move the cursor and the cursor becomes anything but an arrow graphic,
then than particular item on the screen is selectable, e.g.
- 4 small arrows pointing to the centre = Select/Perform action/Go here.
- 1 small arrow with EXIT underneath = Exit out of particular area.
- 1 small arrow with OK underneath = Accept/Select/Perform action/Go here.
- Arrow cursor pointing up/down/left or right = Scroll.

Aliases
-------
Left-Mouse Button = LMB  Right-Mouse Button  = RMB  Middle-Mouse Button = MMB
Mouse Button 'x'  = MBx  Mouse Wheel up      = MWU  Mouse Wheel Down    = MWD
Keypad Button 'x' = KPx  Joystick Button 'x' = JBx  Joystick            = JOY
Mouse             = MSE  Keyboard            = KEY

Anywhere in the game
--------------------
Alt+F4 on Windows or Cmd+Q on a Mac = Instant quit. Any game progress is lost
  but engine settings and ALREADY SAVED game progress will be written to disk
  in a .udb file.
Alt+Enter = Switch between Borderless Full-Screen and decorated Window mode.

Anywhere in the game (but not during a fade transition)
-------------------------------------------------------
F1            = Display this readme document.
F2 or JB8+JB9 = Configure the game engine, video/audio settings, etc.
F11           = Reset the window position and size.
F12           = Take a screenshot (lossless .bmp where .exe file is).
JOY/MSE axes  = Move cursor on screen.
JB1/LMB       = Select control under mouse cursor.

Engine setup screen
-------------------
LMB/JB1         = Scroll readme document at top or bottom of screen.
RMB/JB2/ESCAPE  = Leave readme or setup screen.
MWU/JB2/JB5/JB7 = Select next item.
MWD/JB0/JB4/JB6 = Select precious item.

Intro
-----
ESC/LMB/JB1 = Skip cut-scene/intro.

In-Level
--------
PAUSE or START = Pause and unpause the game.
ESCAPE or START+JB8+JB9 = Forfeit the game only when paused (Game over).
1 or KP1 = Select first digger if alive.
2 or KP2 = Select second digger if alive.
3 or KP3 = Select third digger if alive.
4 or KP4 = Select fourth digger if alive.
5 or KP5 = Select fifth digger if alive.
Backspace = Stop selected digger.
LMB/JB1 = Action, Select or Cancel menu.
RMB/JB2 = Open menu for selected digger.
RETURN = Grab item Digger maybe touching.
UP CURSOR = Jump (When digging left/right, press to dig diagonally up).
DOWN CURSOR = Dig down (When digging left/right, press to dig diag. down).
LEFT CURSOR = Move left (press again to run, press a third time to dig).
RIGHT CURSOR = Move right (press again to run, press a third time to dig).
MINUS KEY or JB8 = Select last Digger.
EQUALS KEY or JB9 = Select next digger.
KP0 = Teleport home/nearest telepole.

In-shop/Race select screen
--------------------------
MWU/MWD = Scroll available options.
LMB = Select item under cursor.

#############################################################################
## For advanced users                                                   07 ##
#############################################################################
If for some reason you're an advanced-user or admin and need to fine tune how
the game engine works, we have some command-line parameters that might be
useful.

Syntax: diggers.exe cvar=value [cvar=value [...]]
  An unlimited amount of cvars can be overriden.
  Examples:-
  * diggers.exe vid_fs=1
    - Will make the engine startup in full-screen.
  * diggers.exe con_disabled=0
    - Will enable the developer console.

You can change cvars by opening up the console with the grave '`' key (key
under ESCAPE key). You will have to start the game with 'con_disabled=0' in
order for this to work. This key may not be available on most keyboards so
you can change it by changing the 'con_key1' or 'con_key2' var. This is a
GLFW specific key code. Once you're in the console, type 'cmdlist' for the
commands you can use or 'cvarlist' for the settings you can change. In the
release version of the game, the console is semi-permanantly disabled, you
can re-enable the console by using "diggers.exe con_disabled=false" to
re-enable it. CVars can obviously be overloaded onto the command-line, not
just one setting.

#############################################################################
## FAQ                                                                  08 ##
#############################################################################
Q. My game controller (joystick) doesn't work?
A. Restart the game with your controller connected or set inp_joystick to 1
   in the console. Note that enabling joystick polling may reduce
   performance.

Q. This does not 'look' like the original game.
A. Well I've had to make a few changes because I want the remake to support
   16:9 widescreen and ive had to modify and adapt most of the old textures
   that were originally in 320x200 (16:10), which is not a 4:3 resolution. If
   you can help upgrade the textures, that would be awesome!

Q. I get an error, strange behaviour or found a bug.
A. You can start the game with the 'log_file=...' (filename) parameter to
   give a pretty detailed log of what the app is doing so you can send me
   that along with as much info as possible such as the .log .crt .dbg and
   .udb files that the app generates. Neither of these files will contain any
   personal information, only technical information to help me squash the
   problem.

Q. Why aren't my settings and game progress being saved?
A. By default, Diggers will try to write this data to the directory where
   the game executable resides in a sqlite database file ending with the
   extension .udb. If this is not possible then an attempt to write to an
   .udb in the users roaming directory (where <Name> is your desktop logon
   username)...
     - Windows = C:\Users\<Name>\AppData\Roaming\MS-Engine\MS-Design\Diggers.
     - MacOSX  = \Users\<Name>\Library\MS-Engine\MS-Design\Diggers.
     - Linux   = \home\<Name>\.MS-Engine\MS-Design\Diggers.
   Remote locations with write-access 'should' work but I have not tested it.
   You can change the location of the .udb config database file by using the
   'app_basedir=...' command line parameter.

Q. I've picked the wrong game-engine settings and the game crashed/won't
   start.
A. Try specifying the parameter 'sql_defaults=1' command-line option to reset
   all the engine settings to default. If this doesn't work you'll have to
   delete the .udb file or set 'sql_defaults=2' and start again from scratch
   thus losing all your Diggers saved game data. All that said though, the
   game shouldn't really crash though so please send me any logs/crash dumps
   you may have.

#############################################################################
## Credits                                                              09 ##
#############################################################################
DIGGERS FAN REMAKE FOR WINDOWS, MACOS AND LINUX >>>>>>>>>>> By Mhat/MS-Design
Uses LibGLFW windowing library ............(c) Marcus Geelnard & Camilla Lowy
Uses LUA scripting engine .............................. (c) Lua.org, PUC-Rio
Uses OpenALSoft 3D audio system ........ Chris Robinson & Creative Technology
Uses Ogg, Vorbis & Theora A/V codec ................................ Xiph.Org
Uses FreeType font rendering ....................... (c) The FreeType Project
Uses SQLite database engine ............................... SQLite Consortium
Uses LibJPEG-Turbo image codec ............... (c) IJG & Contributing authors
Uses LibNSGif image decoder (c) NetSurf Developers, Richard Wilson & Sean Fox
Uses LibPNG image codec .............................(c) Contributing authors
Uses LZMA general codec ......................................... Igor Pavlov
Uses Z-Lib general codec....................(c) Jean-loup Gailly & Mark Adler
Uses OpenSSL crypto & socket engine ............... (c) OpenSSL SW Foundation
Uses RapidJSON parsing engine ...... (c) THL A29 Ltd., Tencent co. & Milo Yip
Setup music (Super Stardust - Special Mission 1 .................. PowerTrace
Credits music (4U 07:00 V2001) ...................................... By Enuo
Gameover music loop (1000 Years Of Funk) ................... By Dimitri D. L.
Special thanks ............................ ModArchive.Org and AmigaRemix.Com
DIGGERS FOR AMIGA, CD32 AND DOS >>>>>>>>>>>>>>>>>>>>>>>>>>>>> By Toby Simpson
Programmed By ..................... Mike Froggatt (PC) & Toby Simpson (Amiga)
Additional DOS Programming ....................................... Keith Hook
Original Design and Amiga Programming .......................... Toby Simpson
Graphics ....................................................... Jason Wilson
Music and Effects .............................. Richard Joseph & Graham King
Additional Graphics ................................. Tony Heap & Jason Riley
Narrative Text ................................................ Martin Oliver
Additional Design .............. Michael Hayward, Ian Saunter & Tony Fagelman
Level Design ................................. Toby Simpson and Tony Fagelman
Quality Assurance .................. Steve Murphy, Paul Dobson & Kelly Thomas
Intro & Outro Sequence ............................ Mike Ball & Mike Froggatt
Book Production ... Alan Brand, Steve Loughran, Matrin Oliver & Tony Fagelman
Thanks . Chris Ludwig, Wayne Lutz, Dave Pocock, Sharon McGuffie & Ben Simpson
Produced By ...................................... Tony Fagelman & Millennium

#############################################################################
## Licence                                                              10 ##
#############################################################################
Diggers is (c) Millennium Interactive Ltd. 1994.

The source code was mainly supplied for reference only so people can
confidently trust the projects I make with it and help me improve it if
necessary. However, it is also public as a result so although I cannot stop
anyone from doing what they want with it, my hope is that if you ONLY do good
with this software, then it would be much appreciated if you could credit me
and by considering sending a donation depending on how well this software
helped you.

MS-Engine is powered by open-source commercially distributional tech. To view
the credits and licences for this software, pull down the console with '`'
and type 'credits' and press return to see them. The licence for a particular
component used can also be read with 'credits n' where 'n' is the id of the
licence in square brackets from the 'credits' output. If you are reading this
document in game then the credits will be listed below.

#############################################################################
## End-of-document                                         End-of-document ##
#############################################################################
