/* == LLINFO.HPP =========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Info' namespace and methods for the guest to use in    ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Info
/* ------------------------------------------------------------------------- */
// ! The info namespace allows the programmer to gain information about the
// ! engine. Check out the Display namespace for more system information
// ! functions when graphical mode is enabled.
/* ========================================================================= */
namespace LLInfo {                     // Info namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace ICmdLine::P;
using namespace ILua::P;               using namespace ISystem::P;
using namespace ITimer::P;             using namespace IUtil::P;
/* ========================================================================= **
** ######################################################################### **
** ## Lua.* namespace functions                                           ## **
** ######################################################################### **
** ========================================================================= */
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
LLFUNCEX(Engine, 12, LCPUSHVAR(cSystem->ENGName(),     cSystem->ENGAuthor(),
                               cSystem->ENGMajor(),    cSystem->ENGMinor(),
                               cSystem->ENGBuild(),    cSystem->ENGRevision(),
                               cSystem->ENGBits(),     cSystem->ENGBuildType(),
                               cSystem->ENGTarget(),   cSystem->ENGCompiled(),
                               cSystem->ENGCompiler(), cSystem->ENGCompVer()));
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
LLFUNCEX(OS, 6, LCPUSHVAR(cSystem->OSName(),  cSystem->OSMajor(),
                          cSystem->OSMinor(), cSystem->OSBuild(),
                          cSystem->OSBits(),  cSystem->OSNameEx()));
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
  cSystem->UpdateMemoryUsageData();
  LCPUSHVAR(cSystem->RAMLoad(), cSystem->RAMTotal(),   cSystem->RAMFree(),
            cSystem->RAMUsed(), cSystem->RAMProcUse(), cSystem->RAMProcPeak());
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
LLFUNCEX(CPU, 7,
  LCPUSHVAR(cSystem->CPUCount(),    cSystem->CPUSpeed(),
            cSystem->CPUFeatures(), cSystem->CPUIdentifier(),
            cSystem->CPUPlatform(), cSystem->CPUName(),
            cSystem->CPUVendor()));
/* ========================================================================= */
// $ Info.LUAUsage
// < Bytes:integer=Bytes of memory.
// ? Returns the amount of memory in use by Lua
/* ------------------------------------------------------------------------- */
LLFUNCEX(LUAUsage, 1, LCPUSHVAR(LuaUtilGetUsage(lS)));
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
LLFUNCEX(CPUUsage, 2,
  LCPUSHVAR(cSystem->UpdateAndGetCPUUsage(), cSystem->CPUUsageSystem()));
/* ========================================================================= */
// $ Info.CPUProcUsage
// < Percent:number=Percentage process.
// ? Returns the engine CPU load. It is hard-coded to only update once a
// ? a second so constant calls won't stress the kernel.
/* ------------------------------------------------------------------------- */
LLFUNCEX(CPUProcUsage, 1, LCPUSHVAR(cSystem->UpdateAndGetCPUUsage()));
/* ========================================================================= */
// $ Info.CPUSysUsage
// < Percent:number=Percentage system.
// ? Returns the system CPU load. It is hard-coded to only update once a
// ? a second so constant calls won't stress the kernel.
/* ------------------------------------------------------------------------- */
LLFUNCEX(CPUSysUsage, 1, LCPUSHVAR(cSystem->UpdateAndGetCPUUsageSystem()));
/* ========================================================================= */
// $ Info.Locale
// < LocaleID:number=Locale id.
// ? Returns the system locale id.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Locale, 1, LCPUSHVAR(cSystem->OSLocale()));
/* ========================================================================= */
// $ Info.OSTime
// < Timestamp:integer=The UNIX timestamp.
// ? Returns a unix timestamp of the current time in seconds.
/* ------------------------------------------------------------------------- */
LLFUNCEX(OSTime, 1, LCPUSHVAR(cmSys.GetTimeS()));
/* ========================================================================= */
// $ Info.OSNumTime
// < Timestamp:number=The UNIX timestamp as a number.
// ? Returns a unix timestamp of the current time in seconds as a number.
/* ------------------------------------------------------------------------- */
LLFUNCEX(OSNumTime, 1,
  LCPUSHVAR(static_cast<lua_Number>(cmSys.GetTimeS<lua_Number>())));
/* ========================================================================= */
// $ Info.OSMilliTime
// < Time:integer=The time in milliseconds.
// ? Returns the time elapsed in milliseconds since the OS started. Precision
// ? will be lost over time.
/* ------------------------------------------------------------------------- */
LLFUNCEX(OSMilliTime, 1, LCPUSHVAR(cmSys.GetTimeMS()));
/* ========================================================================= */
// $ Info.OSNanoTime
// < Time:integer=The time in nanoseconds.
// ? Returns the time elapsed in nanoseconds since the OS started. Precision
// ? will be lost over time.
/* ------------------------------------------------------------------------- */
LLFUNCEX(OSNanoTime, 1, LCPUSHVAR(cmSys.GetTimeNS()));
/* ========================================================================= */
// $ Info.OSMicroTime
// < Time:integer=The time in microseconds.
// ? Returns the time elapsed in microseconds since the OS started. Precision
// ? will be lost over time.
/* ------------------------------------------------------------------------- */
LLFUNCEX(OSMicroTime, 1, LCPUSHVAR(cmSys.GetTimeUS()));
/* ========================================================================= */
// $ Info.Time
// < Time:number=The time in seconds.
// ? Returns the time elapsed in seconds since the engine started. This is a
// ? very high resolution timer. Use it for time-criticial timings. This
// ? counter can be reset with Core.Update() as it will lose precision over
// ? time.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Time, 1, LCPUSHVAR(cmHiRes.GetTimeDouble()));
/* ========================================================================= */
// $ Info.Uptime
// < Ticks:number=Uptime in seconds
// ? Returns the total number of seconds elapsed since the start of the engine
// ? this call maybe a little more expensive than Glfw.Time() but you have no
// ? choice to use this if you are using terminal mode. This uses std::chrono
// ? to retrieve this value.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Uptime, 1, LCPUSHVAR(cLua->CCDeltaToDouble()));
/* ========================================================================= */
// $ Info.Ticks
// < Ticks:integer=Number of ticks.
// ? Returns the total number of frames rendered since the engine started.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Ticks, 1, LCPUSHVAR(cTimer->TimerGetTicks()));
/* ========================================================================= */
// $ Info.CPUFPS
// < FPS:number=Frames per second.
// ? Get CPU loops processed in the last second. Should be the same as GPU for
// ? most people but at times may be different, sometimes much higher.
/* ------------------------------------------------------------------------- */
LLFUNCEX(CPUFPS, 1, LCPUSHVAR(cTimer->TimerGetSecond()));
/* ========================================================================= */
// $ Info.Delay
// < Time:number=Delay time in seconds.
// ? Returns the current thread delay time in seconds without having to read
// ? the 'vid_delay' variable. This would be useful if you are actually using
// ? the delay and you want to offset a time point by the thread delay.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Delay, 1,
  LCPUSHVAR(static_cast<double>(cTimer->TimerGetDelay()) / 1000));
/* ========================================================================= */
// $ Info.IsOSWindows
// < Boolean:boolean=True if using Windows, false if not.
// ? Returns true if executable was built for Windows, false if not.
/* ------------------------------------------------------------------------- */
#if defined(WINDOWS)
LLFUNCEX(IsOSWindows, 1, LCPUSHVAR(true));
#else
LLFUNCEX(IsOSWindows, 1, LCPUSHVAR(false));
#endif
/* ========================================================================= */
// $ Info.IsOSLinux
// < Boolean:boolean=True if using Linux, false if not.
// ? Returns true if executable was built for Linux, false if not.
/* ------------------------------------------------------------------------- */
#if defined(LINUX)
LLFUNCEX(IsOSLinux, 1, LCPUSHVAR(true));
#else
LLFUNCEX(IsOSLinux, 1, LCPUSHVAR(false));
#endif
/* ========================================================================= */
// $ Info.IsOSMac
// < Boolean:boolean=True if using MacOS, false if not.
// ? Returns true if executable was built for MacOS, false if not.
/* ------------------------------------------------------------------------- */
#if defined(MACOS)
LLFUNCEX(IsOSMac, 1, LCPUSHVAR(true));
#else
LLFUNCEX(IsOSMac, 1, LCPUSHVAR(false));
#endif
/* ========================================================================= */
// $ Info.Catchup
// ? Resets the high resolution timer and resets the accumulator. Please be
// ? advised that the video system relies on this timer so videos will not
// ? play properly if this is constantly used. Only use when doing loading
// ? screens.
/* ------------------------------------------------------------------------- */
LLFUNC(Catchup, cTimer->TimerCatchup());
/* ========================================================================= */
// $ Info.Env
// > Value:string=The name of the variable to query.
// < Value:string=The value of the specified variable.
// ? Queries the specified environment variable. Returns a blank string if
// ? empty. All environment variables are converted to upper-case at startup.
// ? Type 'env' in the console to see the current environment.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Env, 1, LCPUSHVAR(cCmdLine->GetEnv(LCGETCPPSTRING(1, "Variable"))));
/* ========================================================================= **
** ######################################################################### **
** ## Info.* namespace functions structure                                ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Info.* namespace functions begin
  LLRSFUNC(Catchup),     LLRSFUNC(CPU),          LLRSFUNC(CPUFPS),
  LLRSFUNC(CPUUsage),    LLRSFUNC(CPUProcUsage), LLRSFUNC(CPUSysUsage),
  LLRSFUNC(Delay),       LLRSFUNC(Engine),       LLRSFUNC(Env),
  LLRSFUNC(Locale),      LLRSFUNC(LUAUsage),     LLRSFUNC(OS),
  LLRSFUNC(IsOSWindows), LLRSFUNC(IsOSLinux),    LLRSFUNC(IsOSMac),
  LLRSFUNC(OSTime),      LLRSFUNC(OSNumTime),    LLRSFUNC(OSMilliTime),
  LLRSFUNC(OSMicroTime), LLRSFUNC(OSNanoTime),   LLRSFUNC(RAM),
  LLRSFUNC(Ticks),       LLRSFUNC(Time),         LLRSFUNC(Uptime),
LLRSEND                                // Info.* namespace functions end
/* ========================================================================= */
}                                      // End of Info namespace
/* == EoF =========================================================== EoF == */
