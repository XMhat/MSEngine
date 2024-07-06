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
using namespace ILog::P;               using namespace ILua::P;
using namespace ISystem::P;            using namespace ITimer::P;
using namespace IUtil::P;              using namespace Common;
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
LLFUNC(Engine, 12, LuaUtilPushVar(lS, cSystem->ENGName(),
  cSystem->ENGAuthor(), cSystem->ENGMajor(), cSystem->ENGMinor(),
  cSystem->ENGBuild(), cSystem->ENGRevision(), cSystem->ENGBits(),
  cSystem->ENGBuildType(), cSystem->ENGTarget(), cSystem->ENGCompiled(),
  cSystem->ENGCompiler(), cSystem->ENGCompVer()))
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
LLFUNC(OS, 6, LuaUtilPushVar(lS, cSystem->OSName(), cSystem->OSMajor(),
  cSystem->OSMinor(), cSystem->OSBuild(), cSystem->OSBits(),
  cSystem->OSNameEx()))
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
LLFUNC(RAM, 6, cSystem->UpdateMemoryUsageData();
  LuaUtilPushVar(lS, cSystem->RAMLoad(), cSystem->RAMTotal(),
    cSystem->RAMFree(), cSystem->RAMUsed(), cSystem->RAMProcUse(),
    cSystem->RAMProcPeak()))
/* ========================================================================= */
// $ Info.CPU
// < CPUid:string=The CPUID string.
// < Count:integer=Number of threads available to the engine.
// < Speed:integer=Processor frequency in Hz.
// < Family:integer=Processor family.
// < Model:integer=Processor model.
// < Stepping:integer=Processor stepping.
// ? Returns information about the installed Central Processing Unit.
/* ------------------------------------------------------------------------- */
LLFUNC(CPU, 6, LuaUtilPushVar(lS, cSystem->CPUName(), cSystem->CPUCount(),
  cSystem->CPUSpeed(), cSystem->CPUFamily(), cSystem->CPUModel(),
  cSystem->CPUStepping()))
/* ========================================================================= */
// $ Info.LUAUsage
// < Bytes:integer=Bytes of memory.
// ? Returns the amount of memory in use by Lua
/* ------------------------------------------------------------------------- */
LLFUNC(LUAUsage, 1, LuaUtilPushVar(lS, LuaUtilGetUsage(lS)))
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
LLFUNC(CPUUsage, 2, LuaUtilPushVar(lS,
  cSystem->UpdateAndGetCPUUsage(), cSystem->CPUUsageSystem()))
/* ========================================================================= */
// $ Info.CPUProcUsage
// < Percent:number=Percentage process.
// ? Returns the engine CPU load. It is hard-coded to only update once a
// ? a second so constant calls won't stress the kernel.
/* ------------------------------------------------------------------------- */
LLFUNC(CPUProcUsage, 1, LuaUtilPushVar(lS, cSystem->UpdateAndGetCPUUsage()))
/* ========================================================================= */
// $ Info.CPUSysUsage
// < Percent:number=Percentage system.
// ? Returns the system CPU load. It is hard-coded to only update once a
// ? a second so constant calls won't stress the kernel.
/* ------------------------------------------------------------------------- */
LLFUNC(CPUSysUsage, 1,
  LuaUtilPushVar(lS, cSystem->UpdateAndGetCPUUsageSystem()))
/* ========================================================================= */
// $ Info.Locale
// < LocaleID:number=Locale id.
// ? Returns the system locale id.
/* ------------------------------------------------------------------------- */
LLFUNC(Locale, 1, LuaUtilPushVar(lS, cSystem->OSLocale()))
/* ========================================================================= */
// $ Info.LUATime
// < Timestamp:number=The execution time as a number.
// ? Returns the total time in the LUA sandbox in seconds.
/* ------------------------------------------------------------------------- */
LLFUNC(LUATime, 1, LuaUtilPushVar(lS, cLua->CCDeltaToDouble()))
/* ========================================================================= */
// $ Info.LUAMilliTime
// < Time:integer=The execution time in milliseconds.
// ? Returns the total time in the LUA sandbox in milliseconds.
/* ------------------------------------------------------------------------- */
LLFUNC(LUAMilliTime, 1, LuaUtilPushVar(lS, cLua->CCDeltaMS()))
/* ========================================================================= */
// $ Info.LUANanoTime
// < Time:integer=The time in nanoseconds.
// ? Returns the total time in the LUA sandbox in nanoseconds.
/* ------------------------------------------------------------------------- */
LLFUNC(LUANanoTime, 1, LuaUtilPushVar(lS, cLua->CCDeltaNS()))
/* ========================================================================= */
// $ Info.LUAMicroTime
// < Time:integer=The time in microseconds.
// ? Returns the total time in the LUA sandbox in microseconds.
/* ------------------------------------------------------------------------- */
LLFUNC(LUAMicroTime, 1, LuaUtilPushVar(lS, cLua->CCDeltaUS()))
/* ========================================================================= */
// $ Info.OSTime
// < Timestamp:integer=The UNIX timestamp.
// ? Returns a unix timestamp of the current time in seconds.
/* ------------------------------------------------------------------------- */
LLFUNC(OSTime, 1, LuaUtilPushVar(lS, cmSys.GetTimeS()))
/* ========================================================================= */
// $ Info.OSNumTime
// < Timestamp:number=The UNIX timestamp as a number.
// ? Returns a unix timestamp of the current time in seconds as a number.
/* ------------------------------------------------------------------------- */
LLFUNC(OSNumTime, 1, LuaUtilPushVar(lS, cmSys.GetTimeS<lua_Number>()))
/* ========================================================================= */
// $ Info.OSMilliTime
// < Time:integer=The time in milliseconds.
// ? Returns the time elapsed in milliseconds since the OS started. Precision
// ? will be lost over time.
/* ------------------------------------------------------------------------- */
LLFUNC(OSMilliTime, 1, LuaUtilPushVar(lS, cmSys.GetTimeMS()))
/* ========================================================================= */
// $ Info.OSNanoTime
// < Time:integer=The time in nanoseconds.
// ? Returns the time elapsed in nanoseconds since the OS started. Precision
// ? will be lost over time.
/* ------------------------------------------------------------------------- */
LLFUNC(OSNanoTime, 1, LuaUtilPushVar(lS, cmSys.GetTimeNS()))
/* ========================================================================= */
// $ Info.OSMicroTime
// < Time:integer=The time in microseconds.
// ? Returns the time elapsed in microseconds since the OS started. Precision
// ? will be lost over time.
/* ------------------------------------------------------------------------- */
LLFUNC(OSMicroTime, 1, LuaUtilPushVar(lS, cmSys.GetTimeUS()))
/* ========================================================================= */
// $ Info.Time
// < Time:number=The time in seconds.
// ? Returns the time elapsed in seconds since the engine started. This is a
// ? very high resolution timer. Use it for time-criticial timings. This
// ? counter can be reset with Core.Update() as it will lose precision over
// ? time.
/* ------------------------------------------------------------------------- */
LLFUNC(Time, 1, LuaUtilPushVar(lS, cmHiRes.GetTimeDouble()))
/* ========================================================================= */
// $ Info.Uptime
// < Ticks:number=Uptime in seconds
// ? Returns the total number of seconds elapsed since the start of the engine
// ? this call maybe a little more expensive than Glfw.Time() but you have no
// ? choice to use this if you are using terminal mode. This uses std::chrono
// ? to retrieve this value.
/* ------------------------------------------------------------------------- */
LLFUNC(Uptime, 1, LuaUtilPushVar(lS, cLog->CCDeltaToDouble()))
/* ========================================================================= */
// $ Info.UpMilliTime
// < Time:integer=The engine uptime in milliseconds.
// ? Returns the total time in the engine has been running in milliseconds.
/* ------------------------------------------------------------------------- */
LLFUNC(UpMilliTime, 1, LuaUtilPushVar(lS, cLog->CCDeltaMS()))
/* ========================================================================= */
// $ Info.UpNanoTime
// < Time:integer=The engine time in nanoseconds.
// ? Returns the total time in the engine has been running in nanoseconds.
/* ------------------------------------------------------------------------- */
LLFUNC(UpNanoTime, 1, LuaUtilPushVar(lS, cLog->CCDeltaNS()))
/* ========================================================================= */
// $ Info.UpMicroTime
// < Time:integer=The engine uptime in microseconds.
// ? Returns the total time the engine has been running in microseconds.
/* ------------------------------------------------------------------------- */
LLFUNC(UpMicroTime, 1, LuaUtilPushVar(lS, cLog->CCDeltaUS()))
/* ========================================================================= */
// $ Info.Ticks
// < Ticks:integer=Number of ticks.
// ? Returns the total number of frames rendered since the engine started.
/* ------------------------------------------------------------------------- */
LLFUNC(Ticks, 1, LuaUtilPushVar(lS, cTimer->TimerGetTicks()))
/* ========================================================================= */
// $ Info.CPUFPS
// < FPS:number=Frames per second.
// ? Get CPU loops processed in the last second. Should be the same as GPU for
// ? most people but at times may be different, sometimes much higher.
/* ------------------------------------------------------------------------- */
LLFUNC(CPUFPS, 1, LuaUtilPushVar(lS, cTimer->TimerGetFPS()))
/* ========================================================================= */
// $ Info.Delay
// < Time:number=Delay time in seconds.
// ? Returns the current thread delay time in seconds without having to read
// ? the 'vid_delay' variable. This would be useful if you are actually using
// ? the delay and you want to offset a time point by the thread delay.
/* ------------------------------------------------------------------------- */
LLFUNC(Delay, 1,
  LuaUtilPushVar(lS, static_cast<lua_Number>(cTimer->TimerGetDelay()) / 1000))
/* ========================================================================= */
// $ Info.IsOSWindows
// < Boolean:boolean=True if using Windows, false if not.
// ? Returns true if executable was built for Windows, false if not.
/* ------------------------------------------------------------------------- */
#if defined(WINDOWS)
LLFUNC(IsOSWindows, 1, LuaUtilPushVar(lS, true))
#else
LLFUNC(IsOSWindows, 1, LuaUtilPushVar(lS, false))
#endif
/* ========================================================================= */
// $ Info.IsOSLinux
// < Boolean:boolean=True if using Linux, false if not.
// ? Returns true if executable was built for Linux, false if not.
/* ------------------------------------------------------------------------- */
#if defined(LINUX)
LLFUNC(IsOSLinux, 1, LuaUtilPushVar(lS, true))
#else
LLFUNC(IsOSLinux, 1, LuaUtilPushVar(lS, false))
#endif
/* ========================================================================= */
// $ Info.IsOSMac
// < Boolean:boolean=True if using MacOS, false if not.
// ? Returns true if executable was built for MacOS, false if not.
/* ------------------------------------------------------------------------- */
#if defined(MACOS)
LLFUNC(IsOSMac, 1, LuaUtilPushVar(lS, true))
#else
LLFUNC(IsOSMac, 1, LuaUtilPushVar(lS, false))
#endif
/* ========================================================================= */
// $ Info.Catchup
// ? Resets the high resolution timer and resets the accumulator. Please be
// ? advised that the video system relies on this timer so videos will not
// ? play properly if this is constantly used. Only use when doing loading
// ? screens.
/* ------------------------------------------------------------------------- */
LLFUNC(Catchup, 0, cTimer->TimerCatchup())
/* ========================================================================= */
// $ Info.Env
// > Value:string=The name of the variable to query.
// < Value:string=The value of the specified variable.
// ? Queries the specified environment variable. Returns a blank string if
// ? empty. All environment variables are converted to upper-case at startup.
// ? Type 'env' in the console to see the current environment.
/* ------------------------------------------------------------------------- */
LLFUNC(Env, 1, LuaUtilPushVar(lS, cCmdLine->GetEnv(AgString{lS,1})))
/* ========================================================================= **
** ######################################################################### **
** ## Info.* namespace functions structure                                ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Info.* namespace functions begin
  LLRSFUNC(CPU),          LLRSFUNC(CPUFPS),       LLRSFUNC(CPUProcUsage),
  LLRSFUNC(CPUSysUsage),  LLRSFUNC(CPUUsage),     LLRSFUNC(Catchup),
  LLRSFUNC(Delay),        LLRSFUNC(Engine),       LLRSFUNC(Env),
  LLRSFUNC(IsOSLinux),    LLRSFUNC(IsOSMac),      LLRSFUNC(IsOSWindows),
  LLRSFUNC(LUAMicroTime), LLRSFUNC(LUAMilliTime), LLRSFUNC(LUANanoTime),
  LLRSFUNC(LUATime),      LLRSFUNC(LUAUsage),     LLRSFUNC(Locale),
  LLRSFUNC(OS),           LLRSFUNC(OSMicroTime),  LLRSFUNC(OSMilliTime),
  LLRSFUNC(OSNanoTime),   LLRSFUNC(OSNumTime),    LLRSFUNC(OSTime),
  LLRSFUNC(RAM),          LLRSFUNC(Ticks),        LLRSFUNC(Time),
  LLRSFUNC(Uptime),       LLRSFUNC(UpMicroTime),  LLRSFUNC(UpMilliTime),
  LLRSFUNC(UpNanoTime),
LLRSEND                                // Info.* namespace functions end
/* ========================================================================= */
}                                      // End of Info namespace
/* == EoF =========================================================== EoF == */
