/* == CORE.HPP ============================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module is the core class which is created by main/WinMain on   ## */
/* ## startup. It is pretty much the game-engine main routines.           ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfCore {                     // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfJson;                // Using json interface
using namespace IfCursor;              // Using cursor interface
using namespace IfMask;                // Using mask interface
using namespace IfDisplay;             // Using display interface
using namespace IfAudio;               // Using audio interface
using namespace IfFile;                // Using file interface
using namespace IfSShot;               // Using sshot interface
using namespace IfCredit;              // Using credit interface
/* ------------------------------------------------------------------------- */
static enum ErrorBehaviour             // Lua error mode behaviour
{ /* ----------------------------------------------------------------------- */
  LEM_IGNORE,                          // Ignore errors and try to continue
  LEM_RESET,                           // Automatically reset on error
  LEM_SHOW,                            // Open console and show error
  LEM_CRITICAL,                        // Terminate engine with error
  LEM_MAX,                             // Maximum number of options supported
} /* ----------------------------------------------------------------------- */
ebErrorMode = LEM_CRITICAL;            // Lua error mode behaviour
/* -- Public Variables ----------------------------------------------------- */
static unsigned int uiErrorCount = 0;  // Number of errors occured
static unsigned int uiErrorLimit = 0;  // Number of errors allowed
/* -- Wait async on all systems -------------------------------------------- */
static void CoreWaitAllAsync(void)
{ // Wait for all asyncronous operations to complete. Bad stuff can happen
  // after we've stopped LUA and 'onload' events are triggering after that!
  cArchives->WaitAsync();              cAssets->WaitAsync();
  cFtfs->WaitAsync();                  cImages->WaitAsync();
  cJsons->WaitAsync();                 cPcms->WaitAsync();
  cStreams->WaitAsync();               cVideos->WaitAsync();
}
/* -- Set lua error mode behaviour ----------------------------------------- */
static CVarReturn CoreErrorBehaviourModified(const unsigned int uiState)
  { return CVarSimpleSetIntNGE(ebErrorMode,
      static_cast<ErrorBehaviour>(uiState), LEM_MAX); }
/* -- Title modified ----------------------------------------------------- */
static CVarReturn CoreTitleModified(const string &strN, string &strV)
{ // Do not allow user to set this variable, only empty is allowed
  if(!strN.empty()) return DENY;
  // Set the title
  strV = Append(cCVars->GetInternalStrSafe(APP_LONGNAME), ' ',
    cCVars->GetInternalStrSafe(APP_VERSION), " (", cSystem->ENGTarget(),
    ")");
  // We changed the value
  return ACCEPT_HANDLED;
}
/* -- Ask system to clear mutex -------------------- Core::SetOneInstance -- */
static CVarReturn CoreClearMutex(const bool bEnabled)
{ // Ignore check if not needed or global mutex creation succeeded
  if(bEnabled)
    cSystem->DeleteGlobalMutex(cCVars->GetInternalStrSafe(APP_LONGNAME));
  // Execution may continue
  return ACCEPT;
}
/* -- Set once instance cvar changed --------------- Core::SetOneInstance -- */
static CVarReturn CoreSetOneInstance(const bool bEnabled)
{ // Ignore check if not needed or global mutex creation succeeded
  if(!bEnabled || cSystem->InitGlobalMutex
    (cCVars->GetInternalStrSafe(APP_LONGNAME)))
      return ACCEPT;
  // Global mutex creation failed so exit program
  throw CmdLine::EC_QUIT;
}
/* -- Set home directory where files are written if base dir dont work ----- */
static CVarReturn CoreSetHomeDir(const string &strP, string &strV)
{ // Home directory
  string strNP;
  // If we need to build a new directory?
  if(strP.empty())
  { // Get roaming directory and if we couldn't get it?
    const string strRDir{ cSystem->GetRoamingDir() };
    if(strRDir.empty()) return ACCEPT_HANDLED;
    // Build new directory. Because this function can only be called before
    // the cvars are loaded and after the app config file is loaded we can
    // only get the app name and author from the initial vars.
    strNP = Format("$/$/$/", strRDir,
      cCVars->GetInitialVar(CVARNAME(APP_AUTHOR), "Unknown Author"),
      cCVars->GetInitialVar(CVARNAME(APP_SHORTNAME), "Unknown Application"));
  } // Use what the user specified
  else strNP = strP;
  // Replace backslashes with forward slashes because backslashes are ugly
  // and the Microsoft Visual C Run-Time accepts backslashes.
  // Try to make the users home directory as an alternative save place.
  if(!DirMkDirEx(PSplitBackToForwardSlashes(strNP)))
    XCL("Failed to setup persistance directory!", "Directory", strNP);
  // Set home directory
  cCmdLine->SetHome(strNP);
  // We are changing the value so make sure the cvar system knows that
  strV = move(strNP);
  return ACCEPT_HANDLED;
}
/* -- Set error limit ------------------------------------------------------ */
static CVarReturn CoreSetResetLimit(const unsigned int uiLimit)
  { return CVarSimpleSetInt(uiErrorLimit, uiLimit); };
/* -- Parses the command-line ---------------------------------------------- */
static CVarReturn CoreParseCmdLine(const string&, string &strV)
{ // Get command line parameters and if we have parameters?
  const StrVector &svArgs = cCmdLine->GetArgList();
  if(!svArgs.empty())
  { // Reserve some memory for building command line
    strV.reserve(32767);
    // Valid commands parsed
    size_t stGood = 0, stArg = 1;
    // Parse command line arguments and iterate through them
    for(const string &strArg : svArgs)
    { // If empty argument
      if(strArg.empty())
      { // Log the failure and continue
        LW(LH_WARNING, "Core rejected empty command-line argument at $!",
          stArg);
      } // Not empty argument?
      else
      { // Tokenise the argument into a key/value pair. We only want a maximum
        // of two tokens, the seperator is allowed on the second token.
        const Token tKeyVal{ strArg, "=", 2 };
        if(tKeyVal.empty())
        { // Log the failure and continue
          LW(LH_WARNING, "Core rejected invalid command-line "
            "argument at $: '$'!", stArg, strArg);
        } // Argument is valid?
        else
        { // Set the variable from command line with full permission because we
          // should allow any variable to be overriden from the command line.
          // Also show an error if the variable could not be set.
          if(cCVars->SetVarOrInitial(tKeyVal[0], tKeyVal.size() <= 1 ?
            strBlank : tKeyVal[1], SCMDLINE|PBOOT, CSC_NOTHING))
          { // Append argument to accepted command line and add a space
            strV.append(strArg);
            strV.append(strSpace);
            // Good variable
            ++stGood;
          } // Failure?
          else
          { // Log the failure
            LW(LH_WARNING, "Core rejected command-line argument at $: '$'!",
              stArg, strArg);
          }
        }
      } // Next argument
      ++stArg;
    } // Remove empty space if not empty
    if(!strV.empty()) strV.pop_back();
    // Free unused memory
    strV.shrink_to_fit();
    // Write command-line arguments parsed
    LW(LH_DEBUG, "Core parsed $ of $ command-line arguments.", stGood, stArg);
  } // No arguments processed
  else LW(LH_DEBUG, "Core parsed no command-line arguments!");
  // We handled setting the variable
  return ACCEPT_HANDLED;
}
/* -- Reset environment function ------------------------------------------- */
static void CoreResetEnvironment(const bool bLeaving)
{ // End current SQL transaction, we need to report it if it succeeded.
  if(cSql->End() != SQLITE_ERROR)
    LW(LH_WARNING, "Core ended an in-progress SQL transaction!");
  // Reset all SQL error codes and stored results and records.
  cSql->Reset();
  // If using graphical inteactive mode?
  if(cSystem->IsGuiMode(GM_GRAPHICS))
  { // Reset main fbo and back clear colour
    cFboMain->ResetClearColour();
    // Reset texture unit and shader program if in GUI mode
    cOgl->ResetBinds();
    // Set main framebuffer as default
    cFboMain->ActivateMain();
    // Reset cursor type, show it and clear input states
    CursorReset();
    cInput->SetCursor(true);
    // Reset main matrix. No need to force a change if it's already set.
    cDisplay->SetDefaultMatrix(false);
    // Cant't disable console if leaving, can if entering
    cConsole->SetCantDisable(bLeaving);
    // Set console enabled if entering.
    if(!bLeaving) cConsole->SetVisible(false);
    // Restore console font properties
    cConsole->RestoreDefaultProperties();
  } // Bot mode? Clear status texts
  else cConsole->ClearStatus();
  // Restore delay timer since we let Lua modify it
  TimerRestoreDelay();
  // Force a suspend when leaving sandbox if suspend is disabled
  if(bLeaving) TimerSetDelayIfZero();
  // Reset timer
  TimerCatchup();
  // Clear lingering events
  cEvtMain->Flush();
}
/* -- Fired when Lua enters the sandbox ------------------------------------ */
static int CoreThreadSandbox(lua_State*const lS)
{ // Capture exceptions...
  try
  { // Clear thread exit status
    cEvtMain->ThreadCancelExit();
    // Compare event code. Do we need to execute scripts?
    switch(cEvtMain->GetExitReason())
    { // Do not re-initialise anything if we processed a LUA error code
      case EMC_LUA_ERROR: break;
      // Lua execution was ended (e.g. use of the 'lend' command)
      case EMC_LUA_END:
        // Setup lua default environment (libraries, config, etc.)
        cLua->SetupEnvironment();
        // Force timer delay to 1ms to prevent 100% thread use on Main*
        TimerSetDelayIfZero();
        // Exceptions from here on are recoverable
        cEvtMain->SetExitReason(EMC_LUA_ERROR);
        // Done
        break;
      // The thread was re-initialised? (e.g. vreset command)
      case EMC_QUIT_THREAD:
        // Tell guest scripts to redraw their fbo's
        cEvtMain->Add(EMC_LUA_REDRAW);
        // Exceptions from here on are recoverable
        cEvtMain->SetExitReason(EMC_LUA_ERROR);
        // Done
        break;
      // For anything else
      default:
        // Reset environment (entering)
        CoreResetEnvironment(false);
        // Setup lua default environment (libraries, config, etc.)
        cLua->SetupEnvironment();
        // Default event code is error status. This is so if even c++
        // exceptions or C (LUA) exceptions occur, the underlying scope knows
        // to handle the error and try to recover. The actual loops will set
        // this to something different when they cleanly exit their loops.
        cEvtMain->SetExitReason(EMC_LUA_ERROR);
        // Execute startup script
        LuaCodeExecFile(lS, cCVars->GetInternalStrSafe(LUA_SCRIPT));
        // Done
        break;
    } // Which gui mode are we in?
    switch(cSystem->GetGuiMode())
    { // Bot mode or bot mode with audio selected?
      case GM_TEXT_NOAUDIO: case GM_TEXT_AUDIO:
      { // Until thread says we should break loop.
        while(cEvtMain->HandleSafe())
        { // Calculate time elapsed in this tick
          TimerUpdateBot();
          // Execute the main tick
          cLua->ExecuteMain();
          // Process bot console
          cConsole->FlushToLog();
        } // Done
        break;
      }// Interactive mode with accumulator?
      case GM_GRAPHICS:
      { // Initialise accumulator for first time
        TimerUpdateInteractive();
        // Threading not enabled?
        if(cDisplay->FlagIsClear(DF_THREADED))
        { // Loop until event manager says we should break
          while(cEvtMain->HandleSafe() && !cGlFW->WinShouldClose())
          { // Process window event manager commands from other threads
            cEvtWin->ManageUnsafe();
            // Is it time to execute a game tick?
            if(TimerShouldTick())
            { // Render the console fbo (if update requested)
              cConsole->Render();
              // Render video textures (if any)
              VideoRender();
              // Loop point incase we need to catchup game ticks
              TimerCatchupUnthreaded:
              { // Process window events
                cGlFW->PollEvents();
                // Set main fbo by default on each frame
                cFboMain->ActivateMain();
                // Poll joysticks
                cInput->PollJoysticks();
                // Execute a tick for each frame missed
                cLua->ExecuteMain();
                // If we should keep catching up frames?
                if(TimerShouldTick())
                { // Flush the main fbo as we're not drawing it yet
                  cFboMain->RenderFbosAndFlushMain();
                  // Render again until we've caught up
                  goto TimerCatchupUnthreaded;
                } // We've completed catching up at this point
              } // Add console fbo to render list
              cConsole->RenderToMain();
              // Render all fbos and copy the main fbo to screen
              cFboMain->Render();
            } // Update timer
            TimerUpdateInteractive();
          }
        } // Loop until event manager says we should break
        else while(cEvtMain->HandleSafe())
        { // Is it time to execute a game tick?
          if(TimerShouldTick())
          { // Render the console fbo (if update requested)
            cConsole->Render();
            // Render video textures (if any)
            VideoRender();
            // Loop point incase we need to catchup game ticks
            TimerCatchupThreaded:
            { // Set main fbo by default on each frame
              cFboMain->ActivateMain();
              // Poll joysticks
              cInput->PollJoysticks();
              // Execute a tick for each frame missed
              cLua->ExecuteMain();
              // If we should keep catching up frames?
              if(TimerShouldTick())
              { // Flush the main fbo as we're not drawing it yet
                cFboMain->RenderFbosAndFlushMain();
                // Render again until we've caught up
                goto TimerCatchupThreaded;
              } // We've completed catching up at this point
            } // Add console fbo to render list
            cConsole->RenderToMain();
            // Render all fbos and copy the main fbo to screen
            cFboMain->Render();
          } // Update timer
          TimerUpdateInteractive();
        } // Done
        break;
      } // What?
      default: XC("Unknown UI mode!", "Mode", cSystem->GetGuiMode());
    }
  } // exception occured so throw LUA stackdump and leave the sandbox
  catch(const exception &E)
  { // Allow Lue to process error. WARNING!! This prevents destructors on
    // all statically initialised classes to NEVER call so make sure we do not
    // statically create something above!
    ProcessError(lS, E.what());
  } // Returning nothing
  return 0;
}
/* -- Lua deinitialiser helper which updates all the classes that use it --- */
static void CoreLuaDeInitHelper(void)
{ // Unregister all lua cvars and console commands
  cCVars->UnregisterAllLuaCVars();
  cConsole->UnregisterAllLuaCommands();
  // De-init lua and update systems that use Lua
  cLua->DeInit();
  // Reset environment (leaving)
  CoreResetEnvironment(true);
}
/* -- DeInitialise engine components --------------------------------------- */
static void CoreDeInitComponents(void) try
{ // Log reason for deinit
  LW(LH_DEBUG, "Engine de-initialising interfaces with code $.",
    cEvtMain->GetExitReason());
  // Unregister exit events
  cEvtMain->DeInit();
  // Whats the exit reason code?
  switch(cEvtMain->GetExitReason())
  { // Quitting thread?
    case EMC_QUIT_THREAD:
      // Not interactive mode? Nothing to de-initialise
      if(cSystem->IsNotGuiMode(GM_GRAPHICS)) return;
      // Unload console background and font textures
      cConsole->DeInitTextureAndFont();
      // Unload font, texture, videos and curor textures
      VideoDeInitTextures();
      FontDeInitTextures();
      TextureDeInitTextures();
      CursorDeInit();
      // Done
      break;
    // Were exiting to de-initialise everything
    default:
      // De-initialise Lua
      CoreLuaDeInitHelper();
      // DeInitialise console class
      cConsole->DeInit();
      // DeInitialise freetype
      cFreeType->DeInit();
      // Return if audio subsystem not enabled
      if(cSystem->IsNotGuiMode(GM_TEXT_AUDIO)) return;
      // DeInitialise audio class
      cAudio->DeInit();
      // Return if not interactive mode
      if(cSystem->IsNotGuiMode(GM_GRAPHICS)) return;
      // De-initialise input
      cInput->DeInit();
      // Done
      break;
  } // Unload all fbos (NOT destroy);
  cFboMain->DeInitAllObjectsAndBuiltIns();
  // De-init core shaders
  cFboBase->DeInitShaders();
  // OpenGL de-initialised (do not throw error if de-initialised)
  cOgl->DeInit(true);
  // Close window
  cGlFW->WinHide();
} // exception occured?
catch(const exception &E)
{ // Make sure the exception is logged
  LW(LH_ERROR, "(ENGINE THREAD DE-INIT EXCEPTION) $", E.what());
}
/* -- Redraw the frame buffer when error occurs ---------------------------- */
static void CoreForceRedrawFrameBuffer(const bool bAndConsole)
{ // Flush log and return if in bot mode
  if(cSystem->IsNotGuiMode(GM_GRAPHICS)) return cConsole->FlushToLog();
  // Reset opengl binds to defaults just incase any were selected
  cOgl->ResetBinds();
  // Render the console?
  if(bAndConsole) cConsole->RenderNow();
  // Render all fbos and copy the main fbo to screen
  cFboMain->Render();
}
/* -- De-initialise everything --------------------------------------------- */
static void CoreDeInitEverything(void)
{ // De-initialise components
  CoreDeInitComponents();
  // If not in graphical mode, we're done
  if(cSystem->IsNotGuiMode(GM_GRAPHICS)) return;
  // Window should close as well
  cGlFW->WinSetClose(GLFW_TRUE);
  // Still bugged in 3.3.2 full-screen windows
  cGlFW->ForceEventHack();
}
/* -- Engine thread (member function) -------------------------------------- */
static int CoreThreadMain(Thread&) try
{ // Log reason for init
  LW(LH_INFO, "Core engine thread started (C:$;M:$<$>).",
    cEvtMain->GetExitReason(), cSystem->GetGuiModeString(),
    cSystem->GetGuiMode());
  // Non-interactive mode?
  if(cSystem->IsNotGuiMode(GM_GRAPHICS))
  { // If we're not initialising for the first time?
    if(cEvtMain->IsExitReason(EMC_NONE))
    { // Initialise freetype and console
      cFreeType->Init();
      cConsole->Init(false);
    } // Initialising for first time? Just update window icons
    else cDisplay->UpdateIcons();
    // With audio mode enabled? Initialise audio class.
    if(cSystem->IsGuiMode(GM_TEXT_AUDIO)) cAudio->Init();
  } // Init interactive console
  else
  { // Set context current and pass selected refresh rate
    cOgl->Init(cDisplay->GetRefreshRate());
    // Initialise core shaders
    cFboBase->InitShaders();
    // If we're not initialising for the first time?
    if(cEvtMain->IsNotExitReason(EMC_NONE))
    { // Reconfigure matrix
      cDisplay->ForceReInitMatrix();
      // Reset window icon
      cDisplay->UpdateIcons();
      // Reload cursor, fbo, console, fonts, textures and videos objects
      CursorReInit();
      FboReInit();
      cConsole->ReInitTextureAndFont();
      FontReInitTextures();
      TextureReInitTextures();
      VideoReInitTextures();
    } // Not initialising for first time
    else
    { // Initialise freetype, console, audio and input classes
      cFreeType->Init();
      cConsole->Init(true);
      cAudio->Init();
      cInput->Init();
    }
  } // Register exit events
  cEvtMain->Init();
  // Lua loop with initialisation. Compare event code
  SandBoxInit: switch(cEvtMain->GetExitReason())
  { // Ignore LUA initialisation if we're re-initialising other components
    case EMC_QUIT_THREAD: break;
    // Any other code will initialise LUA
    default: cLua->Init();
  } // Lua loop without initialisation. Begin by capturing exceptions
  SandBox: try
  { // ...and enter sand box mode. Below function is when we're in sandbox
    cLua->EnterSandbox(CoreThreadSandbox);
  } // ...and if exception occured?
  catch(const exception &E)
  { // Show error in console
    cConsole->AddLine(E.what(), COLOUR_LGRAY);
    // Disable garbage collector so no shenangians while we reset environment.
    cLua->StopGC();
    // Reset glfw errorlevel
    cGlFW->ResetErrorLevel();
    // Check event code that was set?
    switch(cEvtMain->GetExitReason())
    { // Run-time error?
      case EMC_LUA_ERROR:
        // If we are not in the exit script?
        if(!cLua->Exiting())
        { // Compare error mode behaviour
          switch(ebErrorMode)
          { // Ignore errors and try to continue? Execute again
            case LEM_IGNORE:
              // Write ignore exception to log and pause if at the limit.
              if(uiErrorCount >= uiErrorLimit) goto Pause;
              LW(LH_ERROR, "Core sandbox ignored #$/$ run-time exception: $",
                ++uiErrorCount, uiErrorLimit, E.what());
              // Redraw the console but do not show it.
              CoreForceRedrawFrameBuffer(false);
              // Go back into the sandbox.
              goto SandBox;
            // Automatically reset on error?
            case LEM_RESET:
              // Write ignore exception to log and pause if at the limit.
              if(uiErrorCount >= uiErrorLimit) goto Pause;
              LW(LH_ERROR,
                "Core sandbox reset #$/$ with run-time exception: $",
                  ++uiErrorCount, uiErrorLimit, E.what());
              // Flush events and restart the guest
              cLua->ReInit();
              // Redraw the console but do not show it
              CoreForceRedrawFrameBuffer(false);
              // Go back into the sandbox
              goto SandBox;
            // Open console and show error? Just break to other code.
            case LEM_SHOW:
              // Write ignore exception to log.
              Pause: LW(LH_ERROR,
                "Core sandbox run-time exception: $", E.what());
              // Add event to pause.
              cEvtMain->Add(EMC_LUA_PAUSE);
              // Redraw the console and show it.
              CoreForceRedrawFrameBuffer(true);
              // Break to pause
              goto SandBox;
            // Unknown value so report it!
            default: LW(LH_ERROR,
              "Core exception with unknown behaviour $: $",
                ebErrorMode, E.what());
            // Terminate engine with error? Throw to critical error dialog.
            case LEM_CRITICAL:
              // Redraw the console.
              CoreForceRedrawFrameBuffer(false);
              // Throw to critical error dialog
              throw;
          } // Should not get here
        }
        // Grab the exit code from events if the error because it wasn't
        // able to be caught in the events queue and fall through so the
        // original request can be process.
        cEvtMain->UpdateConfirmExit();
        // Check exit code. These are originally set in EvtMain::DoHandle()
        // so make sure all used exit values with ConfirmExit() are checked
        // for here.
        switch(cEvtMain->GetExitReason())
        { // Lua is ending execution? (i.e. via 'lend') fall through.
          case EMC_LUA_END:
          // Lua executing is re-initialising? (i.e. lreset).
          case EMC_LUA_REINIT:
            // Write exception to log.
            LW(LH_ERROR, "Core sandbox de-init exception: $", E.what());
            // Break
            break;
          // Unknown exit reason. Again check EvtMain::DoHandle().
          default: LW(LH_DEBUG,
            "Core has unknown exit reason of $!", cEvtMain->GetExitReason());
          // Quitting and restarting?
          case EMC_QUIT_RESTART:
          case EMC_QUIT_RESTART_NP:
          // Thread and main thread should quit so tell thread to break.
          case EMC_QUIT: throw;
        } // We get here if we're going to the choices
        break;
      // Unknown exit reason so report it and fall through
      default: LW(LH_WARNING,
        "Core has unknown exit reason of $!", cEvtMain->GetExitReason());
      // Lua is ending execution? Shouldn't happen. Fall through.
      case EMC_LUA_END:
      // Lua executing is re-initialising? Shouldn't happen. Fall through.
      case EMC_LUA_REINIT:
      // Quitting and restarting? Shouldn't happen. Fall through.
      case EMC_QUIT_RESTART: case EMC_QUIT_RESTART_NP:
      // Thread and main thread should quit. De-init components and rethrow.
      case EMC_QUIT: CoreDeInitComponents(); throw;
    } // We get here to process lua event as normal.
  } // Why did LUA break out of the sandbox?
  switch(cEvtMain->GetExitReason())
  { // Execution ended? (e.g. 'lend' command was used)
    case EMC_LUA_END:
      // Add message to say the execution ended
      cConsole->AddLine("Execution ended! Use 'lreset' to restart.");
      // De-initialis lua
      CoreLuaDeInitHelper();
      // Re-initialise lua and go back into the sandbox
      goto SandBoxInit;
    // Execution re-initialising? (e.g. 'lreset' command was used)
    case EMC_LUA_REINIT:
      // Add message to say the execution is restarting
      cConsole->AddLine("Execution restarting...");
      // De-initialis lua
      CoreLuaDeInitHelper();
      // Re-initialise lua and go back into the sandbox
      goto SandBoxInit;
    // Unknown value so report it and fall through.
    default: LW(LH_WARNING,
      "Core has unknown error behaviour of $!", ebErrorMode);
    // Execution ended because of error. Shouldn't get here. Fall through.
    case EMC_LUA_ERROR:
    // Restarting engine completely? Fall through.
    case EMC_QUIT_RESTART:
    case EMC_QUIT_RESTART_NP:
    // Quitting the engine completely? De-initialise lua and fall through.
    case EMC_QUIT:
    // Restarting engine subsystems. i.e. 'vreset'? Fall through.
    case EMC_QUIT_THREAD: break;
  } // De-initilise everything
  CoreDeInitEverything();
  // Kill thread
  return 1;
} // exception occured out of loop. Fatal so we have to quit
catch(const exception &E)
{ // We will quit since this is fatal
  cEvtMain->SetExitReason(EMC_QUIT);
  // Write exception to log
  LW(LH_ERROR, "(ENGINE THREAD FATAL EXCEPTION) $", E.what());
  // Show error
  cSystem->SysMsgEx("Engine Thread Fatal Exception", E.what(), MB_ICONSTOP);
  // De-inti everything
  CoreDeInitEverything();
  // Kill thread
  return 2;
}
/* -- Engine should continue? ---------------------------------------------- */
static bool CoreShouldEngineContinue(void)
{ // Compare exit value
  switch(cEvtMain->GetExitReason())
  { // Engine was requested to quit or restart? NO!
    case EMC_QUIT:
    case EMC_QUIT_RESTART:
    case EMC_QUIT_RESTART_NP: return false;
    // Systems were just re-initialising? YES!
    default: return true;
  }
}
/* -- Main function -------------------------------------------------------- */
static int CoreMain(const int iArgs, ARGTYPE**const lArgs,
  ARGTYPE**const lEnv) try
{ // Set this thread's name for debugger
  SysSetThreadName("main");
  // Due to limitations in C++ right now we don't want to initialise our
  // subsystems as static global classes as there is no real way to safely
  // error check these classes initialising so we use global pointers to the
  // static classes below instead. This means we need to use a de-init helper
  // using a simple destructor in order to nullptr the pointer to the class so
  // that it is easier for the engine to crash if we accidentally use the
  // subsystems after the static class below has been freed to prevent use of
  // freed memory. The following is a helper macro to create a static class,
  // assign the global pointer version to it (makes compiler optimise the
  // pointers to references) and clear the pointer when the scope is lost.
#define INITSS(x, ...) DEINITHELPER(dih ## x, c ## x = nullptr); \
    x eng ## x{ __VA_ARGS__ }; c ## x = &eng ## x
  // Initialise critical command-line and logging systems. We cannot really
  // do anything else special until we've enabled these subsystems.
  INITSS(CmdLine, iArgs, lArgs, lEnv); // cppcheck-suppress danglingLifetime
  INITSS(Log);                         // cppcheck-suppress danglingLifetime
  // Capture exceptions so we can log any errors
  try
  { // Initialise other systems. This order should strictly match the #include
    // order in 'msengine.cpp' and not never use these classes outside of the
    // construction and destruction order!
    INITSS(Stats);                     // cppcheck-suppress danglingLifetime
    INITSS(Threads);                   // cppcheck-suppress danglingLifetime
    INITSS(EvtMain);                   // cppcheck-suppress danglingLifetime
    INITSS(System);                    // cppcheck-suppress danglingLifetime
    INITSS(DyLibs);                    // cppcheck-suppress danglingLifetime
    INITSS(LuaFuncs);                  // cppcheck-suppress danglingLifetime
    INITSS(Archives);                  // cppcheck-suppress danglingLifetime
    INITSS(Assets);                    // cppcheck-suppress danglingLifetime
    INITSS(Crypt);                     // cppcheck-suppress danglingLifetime
    INITSS(Sql);                       // cppcheck-suppress danglingLifetime
    INITSS(GlFW);                      // cppcheck-suppress danglingLifetime
    INITSS(CVars);                     // cppcheck-suppress danglingLifetime
    INITSS(Images);                    // cppcheck-suppress danglingLifetime
    INITSS(ImageFmts);                 // cppcheck-suppress danglingLifetime
    INITSS(Pcms);                      // cppcheck-suppress danglingLifetime
    INITSS(PcmFmts);                   // cppcheck-suppress danglingLifetime
    INITSS(FreeType);                  // cppcheck-suppress danglingLifetime
    INITSS(Ftfs);                      // cppcheck-suppress danglingLifetime
    INITSS(Files);                     // cppcheck-suppress danglingLifetime
    INITSS(Masks);                     // cppcheck-suppress danglingLifetime
    INITSS(Jsons);                     // cppcheck-suppress danglingLifetime
    INITSS(Sockets);                   // cppcheck-suppress danglingLifetime
    INITSS(Bins);                      // cppcheck-suppress danglingLifetime
    // Audio rendering subsystems
    INITSS(Oal);                       // cppcheck-suppress danglingLifetime
    INITSS(Audio);                     // cppcheck-suppress danglingLifetime
    INITSS(Sources);                   // cppcheck-suppress danglingLifetime
    INITSS(Samples);                   // cppcheck-suppress danglingLifetime
    INITSS(Streams);                   // cppcheck-suppress danglingLifetime
    // Graphics rendering and window subsystems
    INITSS(Ogl);                       // cppcheck-suppress danglingLifetime
    INITSS(Shaders);                   // cppcheck-suppress danglingLifetime
    INITSS(EvtWin);                    // cppcheck-suppress danglingLifetime
    INITSS(Display);                   // cppcheck-suppress danglingLifetime
    INITSS(Cursors);                   // cppcheck-suppress danglingLifetime
    INITSS(Input);                     // cppcheck-suppress danglingLifetime
    INITSS(FboBase);                   // cppcheck-suppress danglingLifetime
    INITSS(Fbos);                      // cppcheck-suppress danglingLifetime
    INITSS(FboMain);                   // cppcheck-suppress danglingLifetime
    INITSS(SShot);                     // cppcheck-suppress danglingLifetime
    INITSS(Textures);                  // cppcheck-suppress danglingLifetime
    INITSS(Fonts);                     // cppcheck-suppress danglingLifetime
    INITSS(Videos);                    // cppcheck-suppress danglingLifetime
    // Lua always has to be last so it can clean up properly
    INITSS(Console);                   // cppcheck-suppress danglingLifetime
    INITSS(Lua);                       // cppcheck-suppress danglingLifetime
    // Done with this macro
#undef INITSS
    // Codec helper macro. Do not change the order or you will have to update
    // the index and order in Image::LoadFlags or Pcm::LoadFlags respectively.
#define INITCODEC(x) const Codec ## x engCodec ## x
    // Initialise available image codecs. Note, these have to match the order
    // of auto-detection in the image.hpp:LoadData() function...
    INITCODEC(TGA); /* #0 */ INITCODEC(JPG); /* #1 */  INITCODEC(PNG); /* #2 */
    INITCODEC(GIF); /* #3 */ INITCODEC(DDS); /* #4 */
    // Initialise available audio codecs. Note, these have to match the order
    // of auto-detection in the pcm.hpp:LoadData() function...
    INITCODEC(WAV); /* #0 */ INITCODEC(CAF); /* #1 */  INITCODEC(OGG); /* #2 */
    INITCODEC(MP3); /* #3 */
    // Done with this macro
#undef INITCODEC
    // Register default cvars and pass over the current gui mode by ref. All
    // the core parts of the engine are initialised from cvar callbacks.
    cCVars->RegisterDefaults();
    // Enter text mode if ui mode id is below the one specified
    if(cSystem->IsNotGuiMode(GM_GRAPHICS))
    { // Bail out if logging to standard output because this will prevent
      // the text mode from working properly
      if(cLog->IsRedirectedToDevice())
        XC("Text console cannot be used when logging to standard output!");
      // Init lightweight text mode console for monitoring.
      INITHELPER(SysConIH,
        cSystem->SysConInit(cCVars->GetInternalCStrSafe(APP_TITLE),
          cCVars->GetInternalSafe<unsigned int>(CON_TMCCOLS),
          cCVars->GetInternalSafe<unsigned int>(CON_TMCROWS),
          cCVars->GetInternalSafe<bool>(CON_TMCNOCLOSE));
        cSystem->WindowInitialised(nullptr),
        cEvtMain->ThreadDeInit();
        cSystem->SetWindowDestroyed();
        cSystem->SysConDeInit());
      // Update icons if cvars callbacks loaded any.
      cDisplay->UpdateIcons();
      // Perform the initial draw of the console.
      cConsole->FlushToLog();
      // Execute main function until EMC_QUIT or EMC_QUIT_RESTART is passed.
      // We are using the system's main thread so we just need to name this
      // thread properly. We won't actually be spawning a new thread with this
      // though, it's just used as simple exit condition flag to be compatible
      // with the GUI mode.
      while(CoreShouldEngineContinue()) CoreThreadMain(*cEvtMain);
      // Return if system says we don't have to close as quickly as possible
      if(cSystem->SysConIsClosing())
      { // Quickly save cvars, database and log, this is the priority since
        // Windows has a hardcoded termination time for console apps.
        cCVars->Save();
        cSql->DeInit();
        cLog->DeInitSafe();
        // Now Windows can exit anytime it wants
        cSystem->SysConCanCloseNow();
      }
    } // Else were in graphical interactive mode
    else
    { // Initialise Glfw mode and de-init it when exiting
      INITHELPER(GlFWIH, cGlFW->Init(), cGlFW->DeInit());
      // Until engine should terminate.
      while(CoreShouldEngineContinue()) try
      { // Init window and de-init lua envifonment and window on scope exit
        INITHELPER(DIH, cDisplay->Init(),
          cEvtMain->ThreadDeInit();
          cDisplay->DeInit());
        // Clear window manager events list
        cEvtWin->Flush();
        // If threading enabled
        if(cCVars->GetInternalSafe<bool>(WIN_THREAD))
        { // Using window threading
          cDisplay->FlagSet(DF_THREADED);
          // Setup main thread and start it
          cEvtMain->ThreadInit(CoreThreadMain, nullptr);
          // Loop until window should close
          while(!cGlFW->WinShouldClose())
          { // Process window event manager commands from other threads
            cEvtWin->ManageSafe();
            // Wait for more window events
            cGlFW->WaitEvents();
          }
        } // Threading disabled?
        else
        { // Not using window threading
          cDisplay->FlagClear(DF_THREADED);
          // Enter sandbox and process scripts
          CoreThreadMain(*cEvtMain);
        } // Window will reopen at this point
      } // Error occured
      catch(const exception &E)
      { // Send to log and show error message to user
        LW(LH_ERROR, "(WINDOW LOOP EXCEPTION) $", E.what());
        // Exit loop so we don't infinite loop
        cEvtMain->SetExitReason(EMC_QUIT);
        // Show error and try to carry on and clean everything up
        cSystem->SysMsgEx("Window Loop Fatal Exception",
          E.what(), MB_ICONSTOP);
      } // Engine should terminate from here-on
    } // Compare engine exit code...
    switch(cEvtMain->GetExitReason())
    { // If we're to restart process with parameters? Set to do so
      case EMC_QUIT_RESTART: throw CmdLine::EC_RESTART;
      // If we're to restart process without parameters? Set to do so.
      case EMC_QUIT_RESTART_NP: throw CmdLine::EC_RESTART_NO_PARAM;
      // Normal exit
      default: break;
    }
  } // Safe loggable exception occured?
  catch(const exception &E)
  { // Send to log and show error message to user
    LW(LH_ERROR, "(MAIN THREAD FATAL EXCEPTION) $", E.what());
    // Show message box
    SysMessage("Main Thread Exception", E.what(), MB_ICONSTOP);
    // Done
    return 1;
  } // Capture exit/restarts (atexit handler will handle this)
  catch(const CmdLine::ExitCommand &ecCommand)
  { // Compare command
    switch(ecCommand)
    { // Just exit? Put in log that this instance is terminating
      case CmdLine::EC_QUIT:
      { // Put event in log
        LW(LH_WARNING, "Core signalled to terminate!");
        // Set auto quit
        cCmdLine->SetRestart(CmdLine::EC_QUIT);
        // Done
        return 2;
      } // Restart without parameters?
      case CmdLine::EC_RESTART_NO_PARAM:
      { // Put event in log
        LW(LH_WARNING, "Core signalled to restart without parameters!");
        // Set auto restart
        cCmdLine->SetRestart(CmdLine::EC_RESTART_NO_PARAM);
        // Done
        return 3;
      } // Restart with parameters
      case CmdLine::EC_RESTART:
      { // Message to send
        LW(LH_WARNING, "Core signalled to restart with $ parameters!",
          cCmdLine->GetTotalCArgs());
        // Have debugging enabled?
        if(cLog->HasLevel(LH_DEBUG))
        { // Create a counter for argument number
          size_t stId = 0;
          // Log each argument that will be sent. Could be wchar_t or char.
          for(const ARGTYPE*const *atListPtr = cCmdLine->GetCArgs();
            const ARGTYPE*const atPtr = *atListPtr; ++atListPtr)
              cLog->WriteStringSafe(LH_DEBUG,
                Format("- Arg $: $.", stId++, S16toUTF(atPtr)));
        } // Set auto restart
        cCmdLine->SetRestart(CmdLine::EC_RESTART);
        // Done
        return 4;
      } // Done
      default: XC("Invalid exit signal!", "Command", ecCommand);
    }
  } // Clean exit
  return 0;
} // Unsafe exception occured?
catch(const exception &E)
{ // Show message box
  SysMessage("Main Init Exception", E.what(), MB_ICONSTOP);
  // Done
  return 5;
}
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */
