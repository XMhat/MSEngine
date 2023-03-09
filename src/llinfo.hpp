/* == LLINFO.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Info' namespace and methods for the guest to use in    ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Info
/* ------------------------------------------------------------------------- */
// ! The info class allows the programmer to gain information about the engine.
// ! Check out the Glfw namespace for more system information functions.
/* ========================================================================= */
LLNAMESPACEBEGIN(Info)                 // Info namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfLua;                 // Using lua interface
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// $ Info.Engine
// < Title:string=Title of engine (normally MS-Engine).
// < Vendor:string=Author of engine (normally MS-Design).
// < Major:integer=Major version number of engine.
// < Minor:integer=Minor version number of engine.
// < Build:integer=Build version number of engine.
// < Revision:integer=Revision version number of engine.
// < Bits:integer=Bits version number of engine (32 or 64).
// < Type:string=Text representation of built type (Release,Alpha,Beta).
// < Target:string=Text string of the type of executable file.
// < Compiled:string=The timestamp of the executable compilation time.
// < Compiler:string=The name of the compiler that built the executable.
// < CompVersion:string=The version of the compiler that built the executable.
// ? Returns version information about the engine.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Engine)
  LCPUSHXSTR(cSystem->ENGName());       LCPUSHXSTR(cSystem->ENGAuthor());
  LCPUSHINT(cSystem->ENGMajor());       LCPUSHINT(cSystem->ENGMinor());
  LCPUSHINT(cSystem->ENGBuild());       LCPUSHINT(cSystem->ENGRevision());
  LCPUSHINT(cSystem->ENGBits());        LCPUSHSTR(cSystem->ENGBuildType());
  LCPUSHSTR(cSystem->ENGTarget());      LCPUSHSTR(cSystem->ENGCompiled());
  LCPUSHSTR(cSystem->ENGCompiler());    LCPUSHSTR(cSystem->ENGCompVer());
LLFUNCENDEX(12)
/* ========================================================================= */
// $ Info.OS
// < Name:string=Operating system type (Windows,Linux,MacOS).
// < Major:integer=Major version number of operating system.
// < Minor:integer=Minor version number of operating system.
// < Build:integer=Build version number of operating system.
// < Platform:integer=Platform version number of operating system (Windows).
// < SPack:integer=Service pack number of operating system (Windows).
// < Suite:integer=Suite number of operating system (Windows).
// < ProdType:integer=Product number of operating system (Windows).
// < Bits:integer=Bits version of operating system.
// < Extra:string=Extra operating system information (e.g. Wine/Kernel).
// ? Returns version information about the operating system.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(OS)
  LCPUSHXSTR(cSystem->OSName());        LCPUSHINT(cSystem->OSMajor());
  LCPUSHINT(cSystem->OSMinor());        LCPUSHINT(cSystem->OSBuild());
  LCPUSHINT(cSystem->OSBits());         LCPUSHXSTR(cSystem->OSNameEx());
LLFUNCENDEX(6)
/* ========================================================================= */
// $ Info.RAM
// < Load:number=% load of total physical memory.
// < Total:integer=Total physical memory installed in system.
// < Free:integer=Available physical memory to engine.
// < Used:integer=Physical memory in use by the system and other apps.
// < ProcUse:integer=The total memory in use by the engine (including Virtual).
// < ProcPeak:integer=The peak memory usage of the engine.
// ? Returns information about physical memory in the computer.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(RAM)
  cSystem->UpdateMemoryUsageData();     LCPUSHNUM(cSystem->RAMLoad());
  LCPUSHINT(cSystem->RAMTotal());       LCPUSHINT(cSystem->RAMFree());
  LCPUSHINT(cSystem->RAMUsed());        LCPUSHINT(cSystem->RAMProcUse());
  LCPUSHINT(cSystem->RAMProcPeak());
LLFUNCENDEX(6)
/* ========================================================================= */
// $ Info.CPU
// < Count:integer=Number of threads available to the engine.
// < Speed:integer=Processor frequency in hz.
// < Feature:integer=Basic features mask.
// < Id:string=Family/Model/Stepping, etc.
// < Platform:integer=Family/Model/Stepping, etc.
// < CPUid:string=The CPUID string.
// < Vendor:string=The CPU vendor string (AuthenticAMD,
// ? Returns information about the installed Central Processing Unit.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(CPU)
  LCPUSHINT(cSystem->CPUCount());       LCPUSHINT(cSystem->CPUSpeed());
  LCPUSHINT(cSystem->CPUFeatures());    LCPUSHXSTR(cSystem->CPUIdentifier());
  LCPUSHINT(cSystem->CPUPlatform());    LCPUSHXSTR(cSystem->CPUName());
  LCPUSHXSTR(cSystem->CPUVendor());
LLFUNCENDEX(7)
/* ========================================================================= */
// $ Info.LUAUsage
// < Bytes:integer=Bytes of memory.
// ? Returns the amount of memory in use by Lua
/* ------------------------------------------------------------------------- */
LLFUNCEX(LUAUsage, 1, LCPUSHINT(GetLuaUsage(lS)));
/* ========================================================================= */
// $ Info.CPUUsage
// < Percent:number=Percentage process.
// < Percent:number=Percentage system.
// ? Returns the engine and system CPU load. Use this instead of the other
// ? two cpu usage functions if you need both values. This omits a second
// ? check to update cpu usage and very slightly more optimal than calling
// ? both cpu usage functions which are hard-coded to only update once a
// ? a second so constant calls won't stress the kernel.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(CPUUsage)
  LCPUSHNUM(cSystem->UpdateAndGetCPUUsage());
  LCPUSHNUM(cSystem->CPUUsageSystem());
LLFUNCENDEX(2)
/* ========================================================================= */
// $ Info.CPUProcUsage
// < Percent:number=Percentage process.
// ? Returns the engine CPU load. It is hard-coded to only update once a
// ? a second so constant calls won't stress the kernel.
/* ------------------------------------------------------------------------- */
LLFUNCEX(CPUProcUsage, 1, LCPUSHNUM(cSystem->UpdateAndGetCPUUsage()));
/* ========================================================================= */
// $ Info.CPUSysUsage
// < Percent:number=Percentage system.
// ? Returns the system CPU load. It is hard-coded to only update once a
// ? a second so constant calls won't stress the kernel.
/* ------------------------------------------------------------------------- */
LLFUNCEX(CPUSysUsage, 1, LCPUSHNUM(cSystem->UpdateAndGetCPUUsageSystem()));
/* ========================================================================= */
// $ Info.Locale
// < LocaleID:number=Locale id.
// ? Returns the system locale id.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Locale, 1, LCPUSHXSTR(cSystem->OSLocale()));
/* ========================================================================= */
// $ Info.OSTime
// < Timestamp:integer=The UNIX timestamp.
// ? Returns a unix timestamp of the current time in seconds.
/* ------------------------------------------------------------------------- */
LLFUNCEX(OSTime, 1, LCPUSHINT(cmSys.GetTimeS()));
/* ========================================================================= */
// $ Info.OSNumTime
// < Timestamp:number=The UNIX timestamp as a number.
// ? Returns a unix timestamp of the current time in seconds as a number.
/* ------------------------------------------------------------------------- */
LLFUNCEX(OSNumTime, 1,
  LCPUSHNUM(static_cast<lua_Number>(cmSys.GetTimeS<lua_Number>())));
/* ========================================================================= */
// $ Info.OSMilliTime
// < Time:integer=The time in milliseconds.
// ? Returns the time elapsed in milliseconds since the OS started. Precision
// ? will be lost over time.
/* ------------------------------------------------------------------------- */
LLFUNCEX(OSMilliTime, 1, LCPUSHINT(cmSys.GetTimeMS()));
/* ========================================================================= */
// $ Info.OSNanoTime
// < Time:integer=The time in nanoseconds.
// ? Returns the time elapsed in nanoseconds since the OS started. Precision
// ? will be lost over time.
/* ------------------------------------------------------------------------- */
LLFUNCEX(OSNanoTime, 1, LCPUSHINT(cmSys.GetTimeNS()));
/* ========================================================================= */
// $ Info.OSMicroTime
// < Time:integer=The time in microseconds.
// ? Returns the time elapsed in microseconds since the OS started. Precision
// ? will be lost over time.
/* ------------------------------------------------------------------------- */
LLFUNCEX(OSMicroTime, 1, LCPUSHINT(cmSys.GetTimeUS()));
/* ========================================================================= */
// $ Info.Time
// < Time:number=The time in seconds.
// ? Returns the time elapsed in seconds since the engine started. This is a
// ? very high resolution timer. Use it for time-criticial timings. This
// ? counter can be reset with Core.Update() as it will lose precision over
// ? time.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Time, 1, LCPUSHNUM(cmHiRes.GetTimeDouble()));
/* ========================================================================= */
// $ Info.Uptime
// < Ticks:number=Uptime in seconds
// ? Returns the total number of seconds elapsed since the start of the engine
// ? this call maybe a little more expensive than Glfw.Time() but you have no
// ? choice to use this if you are using terminal mode. This uses std::chrono
// ? to retrieve this value.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Uptime, 1, LCPUSHNUM(cLua->CCDeltaToDouble()));
/* ========================================================================= */
// $ Info.Ticks
// < Ticks:integer=Number of ticks.
// ? Returns the total number of frames rendered since the engine started.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Ticks, 1, LCPUSHINT(TimerGetTicks()));
/* ========================================================================= */
// $ Info.CPUFPS
// < FPS:number=Frames per second.
// ? Get CPU loops processed in the last second. Should be the same as GPU for
// ? most people but at times may be different, sometimes much higher.
/* ------------------------------------------------------------------------- */
LLFUNCEX(CPUFPS, 1, LCPUSHNUM(TimerGetSecond()));
/* ========================================================================= */
// $ Info.Delay
// < Time:number=Delay time in seconds.
// ? Returns the current thread delay time in seconds without having to read
// ? the 'vid_delay' variable. This would be useful if you are actually using
// ? the delay and you want to offset a time point by the thread delay.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Delay, 1,
  LCPUSHNUM(static_cast<double>(TimerGetDelay()) / 1000));
/* ========================================================================= */
// $ Info.IsOSWindows
// < Boolean:boolean=True if using Windows, false if not.
// ? Returns true if executable was built for Windows, false if not.
/* ------------------------------------------------------------------------- */
#ifdef _WIN32
LLFUNCEX(IsOSWindows, 1, LCPUSHBOOL(true));
#else
LLFUNCEX(IsOSWindows, 1, LCPUSHBOOL(false));
#endif
/* ========================================================================= */
// $ Info.IsOSLinux
// < Boolean:boolean=True if using Linux, false if not.
// ? Returns true if executable was built for Linux, false if not.
/* ------------------------------------------------------------------------- */
#ifdef __linux__
LLFUNCEX(IsOSLinux, 1, LCPUSHBOOL(true));
#else
LLFUNCEX(IsOSLinux, 1, LCPUSHBOOL(false));
#endif
/* ========================================================================= */
// $ Info.IsOSMac
// < Boolean:boolean=True if using MacOS, false if not.
// ? Returns true if executable was built for MacOS, false if not.
/* ------------------------------------------------------------------------- */
#ifdef __APPLE__
LLFUNCEX(IsOSMac, 1, LCPUSHBOOL(true));
#else
LLFUNCEX(IsOSMac, 1, LCPUSHBOOL(false));
#endif
/* ========================================================================= */
// $ Info.Catchup
// ? Resets the high resolution timer and resets the accumulator. Please be
// ? advised that the video system relies on this timer so videos will not
// ? play properly if this is constantly used. Only use when doing loading
// ? screens.
/* ------------------------------------------------------------------------- */
LLFUNC(Catchup, TimerCatchup());
/* ========================================================================= */
// $ Info.Env
// > Value:string=The name of the variable to query.
// < Value:string=The value of the specified variable.
// ? Queries the specified environment variable. Returns a blank string if
// ? empty. All environment variables are converted to upper-case at startup.
// ? Type 'env' in the console to see the current environment.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Env, 1, LCPUSHXSTR(cCmdLine->GetEnv(LCGETCPPSTRING(1, "Variable"))));
/* ========================================================================= */
/* ######################################################################### */
/* ## Info.* namespace functions structure                               ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Info.* namespace functions begin
  LLRSFUNC(Catchup),                   // Reset the accumulator
  LLRSFUNC(CPU),                       // Get CPU info
  LLRSFUNC(CPUFPS),                    // Get CPU frames per second
  LLRSFUNC(CPUUsage),                  // Get CPU Usage (process+system)
  LLRSFUNC(CPUProcUsage),              // Get CPU Usage (process)
  LLRSFUNC(CPUSysUsage),               // Get CPU Usage (system)
  LLRSFUNC(Delay),                     // Get current thread delay time
  LLRSFUNC(Engine),                    // Get engine info
  LLRSFUNC(Env),                       // Get environment variable
  LLRSFUNC(Locale),                    // Get locale ID
  LLRSFUNC(LUAUsage),                  // Get LUA RAM usage
  LLRSFUNC(OS),                        // Get os version information
  LLRSFUNC(IsOSWindows),               // Returns true if using windows
  LLRSFUNC(IsOSLinux),                 // Returns true if using linux
  LLRSFUNC(IsOSMac),                   // Returns true if using macos
  LLRSFUNC(OSTime),                    // Get unix timestamp in seconds as int
  LLRSFUNC(OSNumTime),                 //   " in seconds as number
  LLRSFUNC(OSMilliTime),               //   " in milliseconds as int
  LLRSFUNC(OSMicroTime),               //   " in microseconds as int
  LLRSFUNC(OSNanoTime),                //   " in nanoseconds as int
  LLRSFUNC(RAM),                       // Get RAM info
  LLRSFUNC(Ticks),                     // Get ticks rendered in game loop
  LLRSFUNC(Time),                      // Get engine hi-res time
  LLRSFUNC(Uptime),                    // Get lua execution time in seconds
LLRSEND                                // Info.* namespace functions end
/* ========================================================================= */
LLNAMESPACEEND                         // End of Info namespace
/* == EoF =========================================================== EoF == */
