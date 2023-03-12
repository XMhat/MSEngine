/* == LLCORE.hPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Core' namespace and methods for the guest to use in    ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Core
/* ------------------------------------------------------------------------- */
// ! The core class allows manipulation of the game engine itself.
/* ========================================================================= */
LLNAMESPACEBEGIN(Core)                 // Core namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfCore;                // Using core namespace
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// $ Core.Done
// ? Confirms that you want the engine to exit. This is so you can perform
// ? clean up actions in your own time by setting Core.SetEnd(), then calling
// ? this function to confirm you're done.
/* ------------------------------------------------------------------------- */
LLFUNC(Done, cEvtMain->Add(EMC_LUA_CONFIRM_EXIT));
/* ========================================================================= */
// $ Core.Quit
// ? Terminates the engine process cleanly.
/* ------------------------------------------------------------------------- */
LLFUNC(Quit, cEvtMain->Add(EMC_QUIT));
/* ========================================================================= */
// $ Core.Reset
// < Result:boolean = Was the event sent successfully?
// ? Ends LUA execution, clears the context, and restarts LUA execution. It
// ? will return 'false' if Lua is already re-initialising.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Reset, 1, LCPUSHBOOL(cLua->ReInit()));
/* ========================================================================= */
// $ Core.Pause
// ? Pauses LUA execution. Obviously, you can't resume and must do it manually!
/* ------------------------------------------------------------------------- */
LLFUNC(Pause, cEvtMain->Add(EMC_LUA_PAUSE));
/* ========================================================================= */
// $ Core.Restart
// ? Restarts the engine process cleanly.
/* ------------------------------------------------------------------------- */
LLFUNC(Restart, cEvtMain->Add(EMC_QUIT_RESTART));
/* ========================================================================= */
// $ Core.RestartFresh
// ? Restarts the engine process cleanly without command-line arguments.
/* ------------------------------------------------------------------------- */
LLFUNC(RestartNP, cEvtMain->Add(EMC_QUIT_RESTART_NP));
/* ========================================================================= */
// $ Core.End
// ? Ends LUA execution and enables the console.
/* ------------------------------------------------------------------------- */
LLFUNC(End, cEvtMain->Add(EMC_LUA_END));
/* ========================================================================= */
// $ Core.Log
// > Text:string=The line of text to write to the log.
// ? Writes the specified line of text to the engine log with highest level.
/* ------------------------------------------------------------------------- */
LLFUNC(Log, cLog->LogExSafe(LH_DISABLED, "(Lua) $",
  LCGETSTRING(char, 1, "String")));
/* ========================================================================= */
// $ Core.LogEx
// > Level:integer=The log severity level.
// > Text:string=The line of text to write to the log.
// ? Writes the specified line of text to the engine log. Note that if the
// ? current log level cvar setting is below this then the function does not
// ? log anything.
/* ------------------------------------------------------------------------- */
LLFUNC(LogEx,
  cLog->LogExSafe(LCGETINTLGE(LHLevel, 2, LH_DISABLED, LH_MAX, "Level"),
    "(Lua) $", LCGETSTRING(char, 1, "String")));
/* ========================================================================= */
// $ Core.Delay
// > Miliseconds:integer=Time in seconds.
// ? Delays the engine thread for this amount of time.
/* ------------------------------------------------------------------------- */
LLFUNC(Delay, TimerSuspend(LCGETINT(unsigned int, 1, "Milliseconds")));
/* ========================================================================= */
// $ Core.RestoreDelay
// ? Restores the frame thread suspend value set via cvars
/* ------------------------------------------------------------------------- */
LLFUNC(RestoreDelay, TimerRestoreDelay());
/* ========================================================================= */
// $ Core.SetDelay
// > Time:integer=Milliseconds to delay by each tick
// ? This is the same as updating the cvar 'app_delay' apart from that the cvar
// ? is not updated and not saved.
/* ------------------------------------------------------------------------- */
LLFUNC(SetDelay,
  TimerSetDelay(LCGETNUMLG(unsigned int, 1, 0, 1000, "Milliseconds")));
/* ========================================================================= */
// $ Core.PidRunning
// > Id:integer=The pid number to check
// < Result:boolean=The pid is valid and running?
// ? Asks the operating system to check if the specified pid exists and if the
// ? specified executable matches, true is returned, else false. Specifying
// ? pid zero will cause an exception.
/* ------------------------------------------------------------------------- */
LLFUNCEX(PidRunning, 1,
  LCPUSHBOOL(cSystem->IsPidRunning(LCGETINTLG(unsigned int, 1,
    1, numeric_limits<unsigned int>::max(), "Pid"))));
/* ========================================================================= */
// $ Core.KillPid
// > Pid:integer=The pid of the executable to kill
// < Result:boolean=The process is killed?
// ? Kills the specified process id. This only works on pids that were
// ? originally spawned by the engine. Specifying
// ? pid zero will cause an exception.
/* ------------------------------------------------------------------------- */
LLFUNCEX(KillPid, 1,
  LCPUSHBOOL(cSystem->TerminatePid(LCGETINTLG(unsigned int, 1,
    1, numeric_limits<unsigned int>::max(), "Pid"))));
/* ========================================================================= */
// $ Core.SetIcon
// > Filename:string=The filenames of the large icon to set.
// ? Asks the operating system to set the specified icons of the programmers
// ? choosing. Can be .bmp .jpg .png or .tga. Separate each filename with a
// ? colon ':'. On Windows, anything but the first and the last icon are
// ? dropped so make sure you list the first filename as the large icon and the
// ? last filename as the small icon.
/* ------------------------------------------------------------------------- */
LLFUNC(SetIcon, cDisplay->SetIconFromLua(LCGETCPPSTRING(1, "Filenames")));
/* ========================================================================= */
// $ Core.WaitAsync
// ? Delays main thread execution until ALL asynchronous threads have completed
// ? Please note that the engine will be unresponsive during this time and
// ? therefore this call should only really be used in emergencies. Sockets
// ? are NOT synchronied. Use Sockets.CloseAll() to do that.
/* ------------------------------------------------------------------------- */
LLFUNC(WaitAsync, CoreWaitAllAsync());
/* ========================================================================= */
// $ Core.Stack
// < Stack:string=The current stack trace
// > Message:string=The message to prefix
// ? Returns the current stack as a string formatted by the engine and not Lua
// ? This is needed for example when you use xpcall() with an error handler
// ? Note that pcall() error messages do not include the stack.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Stack, 1, LCPUSHXSTR(LCGETCPPSTRING(1, "Message") + GetStack(lS)));
/* ========================================================================= */
// $ Core.OnTick
// > Func:function=The main tick function to change to
// ? On initialisation of IfLua:: The function address of 'MainTick' is
// ? stored in the LUA registry for quick successive execution. Thus to change
// ? this function, you need to run this command with the function you want to
// ? change to.
/* ------------------------------------------------------------------------- */
LLFUNC(OnTick, LCSETEVENTCB(cLua->lrMainTick));
/* ========================================================================= */
// $ Core.OnEnd
// > Func:function=The main end function to change to
// ? The function address to execute when the engine has been asked to
// ? terminate. The function _MUST_ call Core.Done() when that function has
// ? finished tidying up or the engine will soft-lock. Calling this when the
// ? engine is already terminating will do nothing so use Core.SetMain()
// ? instead if you want to change to a new main tick function.
/* ------------------------------------------------------------------------- */
LLFUNC(OnEnd, LCSETEVENTCB(cLua->lrMainEnd));
/* ========================================================================= */
/* ######################################################################### */
/* ## Core.* namespace functions structure                                ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Core.* namespace functions begin
  LLRSFUNC(Delay),                     // Delay execution
  LLRSFUNC(Done),                      // Confirm exit
  LLRSFUNC(End),                       // Restart LUA without init
  LLRSFUNC(PidRunning),                // Process id is running?
  LLRSFUNC(KillPid),                   // Kill a process pid safely
  LLRSFUNC(Log),                       // Send text to log
  LLRSFUNC(LogEx),                     // Send text to log with level
  LLRSFUNC(Pause),                     // Pause LUA
  LLRSFUNC(Quit),                      // Quit the application
  LLRSFUNC(Reset),                     // Restart LUA
  LLRSFUNC(Restart),                   // Restart engine completely
  LLRSFUNC(RestartNP),                 //   " without command-line args
  LLRSFUNC(RestoreDelay),              // Restore default tick sleep
  LLRSFUNC(Stack),                     // Returns current stack
  LLRSFUNC(SetDelay),                  // Set tick sleep
  LLRSFUNC(SetIcon),                   // Set window icon
  LLRSFUNC(WaitAsync),                 // Wait for all async events
  LLRSFUNC(OnEnd),                     // Update main end tick
  LLRSFUNC(OnTick),                    // Update main tick
LLRSEND                                // Core.* namespace functions end
/* ========================================================================= */
/* ######################################################################### */
/* ## Core.* namespace constants structure                                ## */
/* ######################################################################### */
/* ========================================================================= */
// @ Core.LogLevels
// < Data:table=The entire list of possible log levels.
// ? Returns a table of key/value pairs that identify possible log levels.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(LogLevels)                 // Beginning of log levels
LLRSKTITEM(LH_,DISABLED),              LLRSKTITEM(LH_,ERROR),
LLRSKTITEM(LH_,WARNING),               LLRSKTITEM(LH_,INFO),
LLRSKTITEM(LH_,DEBUG),                 LLRSKTITEM(LH_,MAX),
LLRSKTEND                              // End of log levels
/* ========================================================================= */
LLRSCONSTBEGIN                         // Core.* namespace consts begin
LLRSCONST(LogLevels),                  // Core log levels
LLRSCONSTEND                           // Core.* namespace consts end
/* ========================================================================= */
LLNAMESPACEEND                         // End of Core namespace
/* == EoF =========================================================== EoF == */
