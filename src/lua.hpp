/* == LUA.HPP ============================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## The lua instance is the core scripting module that glues all the    ## **
** ## engine components together. Note that this class does not handle    ## **
** ## and inits or deinits because this has to be done later on as        ## **
** ## lua allocated objects need to destruct their objects before the     ## **
** ## actual object destructors are called.                               ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ILua {                       // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace ICollector::P;
using namespace ICrypt::P;             using namespace ICVarDef::P;
using namespace ICVar::P;              using namespace ICVarLib::P;
using namespace IError::P;             using namespace IEvtMain::P;
using namespace IFlags;                using namespace ILog::P;
using namespace ILuaDef;               using namespace ILuaCode::P;
using namespace ILuaFunc::P;           using namespace ILuaLib::P;
using namespace ILuaUtil::P;           using namespace ISql::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISystem::P;            using namespace ISysUtil::P;
using namespace ITimer::P;             using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Flags for setting up environment ------------------------------------- */
BUILD_FLAGS(Lua,
  /* ----------------------------------------------------------------------- */
  // Load nothing                      // Load engine core api
  LF_NONE                   {Flag[0]}, LF_ENGINE                 {Flag[1]},
  // Load lua core api
  LF_CORE                   {Flag[2]},
  /* -- Mask bits ---------------------------------------------------------- */
  LF_MASK{ LF_ENGINE|LF_CORE }
);/* ----------------------------------------------------------------------- */
/* == Lua class ============================================================ */
static class Lua final :
  /* -- Base classes ------------------------------------------------------- */
  public ClockChrono<CoreClock>,       // Runtime clock
  public LuaFlags,                     // Lua startup flags
  private EvtMain::RegVec              // Events list to register
{ /* -- Private typedefs --------------------------------------------------- */
  typedef unique_ptr<lua_State, function<decltype(lua_close)>> LuaPtr;
  /* -- Private variables -------------------------------------------------- */
  LuaPtr           lsState;            // Lua state pointer
  bool             bExiting;           // Ending execution?
  int              iOperations;        // Default ops before timeout check
  int              iStack;             // Default stack size
  int              iGCPause;           // Default GC pause time
  int              iGCStep;            // Default GC step counter
  lua_Integer      liSeed;             // Default seed
  /* -- References ------------------------------------------------- */ public:
  LuaFunc          lrMainTick;         // Main tick function callback
  LuaFunc          lrMainEnd;          // End function callback
  LuaFunc          lrMainRedraw;       // Redraw function callback
  /* -- Return lua state --------------------------------------------------- */
  lua_State *GetState(void) const { return lsState.get(); }
  /* -- Set GC step -------------------------------------------------------- */
  CVarReturn SetFlags(const LuaFlagsType lftFlags)
  { // Convert flags and fail if flags are invalid
    if(lftFlags != LF_MASK && (lftFlags & ~LF_MASK)) return DENY;
    // Flags validated so set and return success
    FlagReset(lftFlags);
    // Accepted
    return ACCEPT;
  }
  /* -- Set a lua reference (LuaFunc can't have this check) ---------------- */
  void SetLuaRef(lua_State*const lS, LuaFunc &lrEvent, const int iParam)
  { // Need only one parameter which is the function
    LuaUtilCheckParams(lS, iParam);
    // It must be a function
    LuaUtilCheckFunc(lS, iParam, "Callback");
    // And must be on the main thread
    StateAssert(lS);
    // Set the function
    lrEvent.LuaFuncSet();
  }
  /* -- Ask LUA to tell guest to redraw ------------------------------------ */
  void SendRedraw(const EvtMain::Cell&)
  { // Lua not initialised? This may be executed before Init() via an
    // exception. For example... The CONSOLE.Init() may have raised an
    // exception. Also do not fire if paused
    if(!lsState || bLuaPaused) return;
    // Get ending function and ignore if not a function
    lrMainRedraw.LuaFuncDispatch();
    // Say that we've finished calling the function
    cLog->LogDebugSafe("Lua finished calling redraw execution callback.");
  }
  /* -- Check if we're already exiting ------------------------------------- */
  bool Exiting(void) { return bExiting; }
  /* -- Events asking LUA to quit ------------------------------------------ */
  void AskExit(const EvtMain::Cell &)
  { // Ignore if already exiting
    if(bExiting) return;
    // Resume if paused
    ResumeExecution();
    // Swap end function with main function
    lrMainTick.LuaFuncDeInit();
    lrMainTick.LuaFuncSwap(lrMainEnd);
    // Now it's up to the guest to end execution with Core.Done();
    bExiting = true;
  }
  /* -- Pause execution ---------------------------------------------------- */
  bool PauseExecution(void)
  { // Bail if not initialised or already paused or exiting
    if(!lsState || bLuaPaused || bExiting) return false;
    // Set empty function callbacks
    LuaFuncDisableAllRefs();
    // Paused
    bLuaPaused = true;
    // Success
    return true;
  }
  /* -- Resume execution --------------------------------------------------- */
  bool ResumeExecution(void)
  { // Bail if not initialised or already paused or exiting
    if(!lsState || !bLuaPaused || bExiting) return false;
    // Restore original functions
    LuaFuncEnableAllRefs();
    // Resumed
    bLuaPaused = false;
    // Done
    return true;
  }
  /* -- Stop gabage collection --------------------------------------------- */
  void StopGC(void) const
  { // Garbage collector is running?
    if(LuaUtilGCRunning(GetState()))
    { // Stop garbage collector
      LuaUtilGCStop(GetState());
      // Log success
      cLog->LogDebugSafe("Lua garbage collector stopped.");
    } // Garbage collector not running? Show warning in log.
    else cLog->LogWarningSafe("Lua garbage collector already stopped!");
  }
  /* -- Start garbage collection ------------------------------------------- */
  void StartGC(void) const
  { // Garbage collector is not running?
    if(!LuaUtilGCRunning(GetState()))
    { // Start garbage collector
      LuaUtilGCStart(GetState());
      // Log success
      cLog->LogDebugSafe("Lua garbage collector started.");
    } // Garbage collector running? Show warning in log.
    else cLog->LogWarningSafe("Lua garbage collector already started!");
  }
  /* -- Full garbage collection while logging memory usage ----------------- */
  size_t GarbageCollect(void) const { return LuaUtilGCCollect(GetState()); }
  /* -- Checks that the state matches with main state ---------------------- */
  void StateAssert(lua_State*const lS) const
  { // This function call is needed when some LUA API functions need to make
    // a reference to a variable (i.e. a callback function) on the internal
    // stack, and this cannot be done on a different (temporary) context. So
    // this makes sure that only the specified call can only be made in the
    // main LUA context.
    if(lS == GetState()) return;
    // Throw error to state passed
    XC("Call not allowed in temporary contexts!");
  }
  /* -- Compile a string and display it's result --------------------------- */
  const string CompileStringAndReturnResult(const string &strWhat)
  { // Save time so we can measure performance
    const ClockChrono ccExecute;
    // Save stack position. This restores the position whatever the result and
    // also cleans up the return values.
    const LuaStackSaver lssSaved{ GetState() };
    // Compile the specified script from the command line
    LuaCodeCompileString(GetState(), strWhat, {});
    // Move compiled function for LuaUtilPCall argument
    lua_insert(GetState(), 1);
    // Call the protected function. We don't know how many return values.
    LuaUtilPCall(GetState(), 0, LUA_MULTRET);
    // Scan for results
    StrList slResults;
    for(int iI = lssSaved.Value() + 1; !lua_isnone(GetState(), iI); ++iI)
      slResults.emplace_back(LuaUtilGetStackType(GetState(), iI));
    // Print result
    return slResults.empty() ?
      StrFormat("Request took $.",
        StrShortFromDuration(ccExecute.CCDeltaToDouble())) :
      StrFormat("Request took $ returning $: $.",
        StrShortFromDuration(ccExecute.CCDeltaToDouble()),
        StrPluraliseNum(slResults.size(), "result", "results"),
        StrImplode(slResults, ", "));
  }
  /* -- Execute main function ---------------------------------------------- */
  void ExecuteMain(void) const { lrMainTick.LuaFuncPushAndCall(); }
  /* -- When lua enters the specified function ----------------------------- */
  static void OnInstructionCount(lua_State*const lS, lua_Debug*const)
    { if(cTimer->TimerIsTimedOut()) LuaUtilPushErr(lS, "Frame timeout!"); }
  /* -- When operations count have changed --------------------------------- */
  CVarReturn SetOpsInterval(const int iCount)
    { return CVarSimpleSetIntNL(iOperations, iCount, 1); }
  /* -- Set default size of stack ------------------------------------------ */
  CVarReturn SetStack(const int iValue)
    { return CVarSimpleSetInt(iStack, iValue); }
  /* -- Set GC pause time -------------------------------------------------- */
  CVarReturn SetGCPause(const int iValue)
    { return CVarSimpleSetInt(iGCPause, iValue); }
  /* -- Set GC step -------------------------------------------------------- */
  CVarReturn SetGCStep(const int iValue)
    { return CVarSimpleSetInt(iGCStep, iValue); }
  /* -- Set GC step -------------------------------------------------------- */
  CVarReturn SetSeed(const lua_Integer liV)
    { return CVarSimpleSetInt(liSeed, liV); }
  /* -- Return operations count -------------------------------------------- */
  int GetOpsInterval(void) { return iOperations; }
  /* -- Init lua library and configuration --------------------------------- */
  void SetupEnvironment(void)
  { // Stop the garbage collector
    StopGC();
    // Init references
    LuaFuncInitRef(GetState());
    // Set default end function to automatically exit the engine
    LuaUtilPushCFunc(GetState(), [](lua_State*)->int{
      cEvtMain->Add(EMC_LUA_CONFIRM_EXIT); return 0;
    });
    lrMainEnd.LuaFuncSet();
    // Set initial size of stack
    cLog->LogDebugExSafe("Lua $ stack size to $.",
      LuaUtilIsStackAvail(GetState(), iStack) ?
        "initialised" : "could not initialise", iStack);
    // Set incremental garbage collector settings. We're not using LUA_GCGEN
    // right now as it makes everything lag so investigate it sometime.
    LuaUtilGCSet(GetState(), LUA_GCINC, iGCPause, iGCStep);
    cLog->LogDebugExSafe("Lua initialised incremental gc to $:$.",
      iGCPause, iGCStep);
    // Init engine variables?
    if(FlagIsSet(LF_ENGINE))
    { // Log progress
      cLog->LogDebugSafe("Lua registering engine namespaces...");
      // Counters for logging stats
      int iCount        = 0,           // Number of global namespaces used
          iTotal        = 0,           // Number of global namespaces in total
          iMembers      = 0,           // Number of static functions used
          iMethods      = 0,           // Number of class methods used
          iTables       = 0,           // Number of tables used
          iTotalMembers = 0,           // Number of static functions in total
          iTotalMethods = 0,           // Number of class methods in total
          iTotalTables  = 0;           // Number of tables in total
      // Init core libraries
      for(const LuaLibStatic &llRef : luaLibList)
      { // Increment total statistics
        iTotalMembers += llRef.iLLCount;
        iTotalMethods += llRef.iLLMFCount;
        iTotalTables += llRef.iLLKICount;
        ++iTotal;
        // Ignore if this API is not allowed in the current operation mode
        if(!cSystem->IsCoreFlagsHave(llRef.cfcRequired)) continue;
        // Increment used statistics
        iMembers += llRef.iLLCount;
        iMethods += llRef.iLLMFCount;
        iTables += llRef.iLLKICount;
        ++iCount;
        // Load class creation functions
        LuaUtilPushTable(GetState(), 0,
          llRef.iLLCount + llRef.iLLMFCount + llRef.iLLKICount);
        luaL_setfuncs(GetState(), llRef.libList, 0);
        // If we have consts list?
        if(llRef.lkiList)
        { // Walk through the table
          for(const LuaTable *ltPtr = llRef.lkiList; ltPtr->cpName; ++ltPtr)
          { // Get reference to table
            const LuaTable &ltRef = *ltPtr;
            // Create a table
            LuaUtilPushTable(GetState(), 0, ltRef.iCount);
            // Walk through the key/value pairs
            for(const LuaKeyInt *kiPtr = ltRef.kiList; kiPtr->cpName; ++kiPtr)
            { // Get reference to key/value pair
              const LuaKeyInt &kiRef = *kiPtr;
              // Push the name of the item
              LuaUtilPushInt(GetState(), kiRef.liValue);
              lua_setfield(GetState(), -2, kiRef.cpName);
            } // Set field name and finalise const table
            lua_setfield(GetState(), -2, ltRef.cpName);
          }
        } // If we have don't have member functions?
        if(!llRef.libmfList)
        { // Set this current list to global
          lua_setglobal(GetState(), llRef.strvName.data());
          // Log progress
          cLog->LogDebugExSafe("- $ ($ members and $ tables).",
            llRef.strvName, llRef.iLLCount, llRef.iLLKICount);
          // Continue
          continue;
        } // Load members into this namespace too for possible aliasing.
        luaL_setfuncs(GetState(), llRef.libmfList, 0);
        // Set to global variable
        lua_setglobal(GetState(), llRef.strvName.data());
        // Pre-cache the metadata for the class and it's members. Note that
        // all threads of the same lua state share a single registry so this is
        // very safe. Start by creating a new metadata table for 4 entries.
        LuaUtilPushTable(GetState(), 0, 4);
        // Push the name of the metatable
        LuaUtilPushStrView(GetState(), llRef.strvName);
        lua_setfield(GetState(), -2, "__name");
        // Set methods so var:func() works
        LuaUtilPushTable(GetState(), 0, llRef.iLLMFCount);
        luaL_setfuncs(GetState(), llRef.libmfList, 0);
        lua_setfield(GetState(), -2, "__index");
        // Protect metadata. getmetatable(x) returns the type instead.
        LuaUtilPushStrView(GetState(), llRef.strvName);
        lua_setfield(GetState(), -2, "__metatable");
        // Push destructor function, this is copied to __gc when as classes
        // are constructed. See CreateClass in luautil.hpp.
        LuaUtilPushCFunc(GetState(), llRef.lcfpDestroy);
        lua_setfield(GetState(), -2, "__dtor");
        // Register the metatable
        lua_pushvalue(GetState(), -1);
        lua_setfield(GetState(), LUA_REGISTRYINDEX, llRef.strvName.data());
        // Remove the table (not sure why this table wasn't removed).
        LuaUtilRmStack(GetState());
        // Log progress
        cLog->LogDebugExSafe("- $ ($ members, $ tables and $ methods).",
          llRef.strvName, llRef.iLLCount, llRef.iLLKICount, llRef.iLLMFCount);
        // For some reason, I cannot specify a __gc member for the garbage
        // collector in this metadata part above because LUA seems to remove
        // the __gc for some reason. My guess it is to protect from double
        // clean-ups which is obviously fatal.
      } // Count libraries and report them
      cLog->LogDebugExSafe(
        "Lua registered $ of $ global namespaces...\n"
        "- $ of $ const tables are registered.\n"
        "- $ of $ member functions are registered.\n"
        "- $ of $ method functions are registered.\n"
        "- $ of $ functions are registered in total.",
        iCount,             iTotal,
        iTables,            iTotalTables,
        iMembers,           iTotalMembers,
        iMethods,           iTotalMethods,
        iMembers+iMethods, iTotalMembers+iTotalMethods);
    } // Init enviornment variables?
    if(FlagIsSet(LF_CORE))
    { // Log progress
      cLog->LogDebugSafe("Lua registering core namespaces...");
      // Load default libraries
      luaL_openlibs(GetState());
      // Log progress
      cLog->LogDebugSafe("Lua registered core namespaces.");
      // Initialise random number generator and if pre-defined?
      if(liSeed)
      { // Init pre-defined seed
        LuaUtilInitRNGSeed(GetState(), liSeed);
        // Warn developer/user that there is a pre-defined random seed
        cLog->LogWarningExSafe("Lua using pre-defined random seed '$'!",
          liSeed);
      } // Use a random number instead
      else LuaUtilInitRNGSeed(GetState(), CryptRandom<lua_Integer>());
    } // Standard library not available so we can only set C-Lib seed
    else StdSRand(!!liSeed ? static_cast<unsigned int>(liSeed) :
                             CryptRandom<unsigned int>());
    // Get variables namespace
    lua_getglobal(GetState(), "Variable");
    // Create a table of the specified number of variables
    LuaUtilPushTable(GetState(), 0, CVAR_MAX);
    // Push each cvar id to the table
    lua_Integer liIndex = 0;
    for(const CVarMapIt &cvmiIt : cCVars->GetInternalList())
    { // If stored iterator is valid?
      if(cvmiIt != cCVars->GetVarListEnd())
      { // Push internal id value name
        LuaUtilPushInt(GetState(), liIndex);
        // Assign the id to the cvar name
        lua_setfield(GetState(), -2, cvmiIt->first.c_str());
      } // Next id
      ++liIndex;
    } // Push cvar id table into the core namespace
    lua_setfield(GetState(), -2, "Internal");
    // Remove the table
    LuaUtilRmStack(GetState());
    // Log that we added the variables
    cLog->LogDebugExSafe("Lua published $ engine cvars.",  CVAR_MAX);
    // Use a timeout hook?
    if(iOperations > 0)
    { // Set the hook
      LuaUtilSetHookCallback(GetState(), OnInstructionCount, iOperations);
      // Log that it was enabled
      cLog->LogDebugExSafe("Lua timeout set to $ sec for every $ operations.",
        StrShortFromDuration(cTimer->TimerGetTimeOut(), 1), iOperations);
    } // Show a warning to say the timeout hook is disabled
    else cLog->LogWarningSafe("Lua timeout hook disabled so use at own risk!");
    // Resume garbage collector
    StartGC();
    // Report completion
    cLog->LogDebugSafe("Lua environment initialised.");
  }
  /* -- Enter sandbox mode ------------------------------------------------- */
  void EnterSandbox(lua_CFunction cbFunc, void*const vpPtr)
  { // Push and get error callback function id
    const int iParam = LuaUtilPushAndGetGenericErrId(GetState());
    // Push function and parameters
    LuaUtilPushCFunc(GetState(), cbFunc);
    // Push user parameter (core class)
    LuaUtilPushPtr(GetState(), vpPtr);
    // Call it! No parameters and no returns
    LuaUtilPCallSafe(GetState(), 1, 0, iParam);
  }
  /* -- De-initialise LUA context ------------------------------------------ */
  void DeInit(void)
  { // Return if class already initialised
    if(!lsState) return;
    // Report progress
    cLog->LogDebugSafe("Lua sandbox deinitialising...");
    // De-init instruction count hook?
    LuaUtilSetHookCallback(GetState(), nullptr, 0);
    // Disable garbage collector
    StopGC();
    // Unregister lua related events
    cEvtMain->UnregisterEx(*this);
    // DeInit references
    LuaFuncDeInitRef();
    // Close state and reset var
    lsState.reset();
    // No longer paused or exited
    bLuaPaused = bExiting = false;
    // Report progress
    cLog->LogDebugSafe("Lua sandbox successfully deinitialised.");
  }
  /* -- Default allocator that uses malloc() ------------------------------- */
  static void *LuaDefaultAllocator(void*const, void*const vpAddr,
    size_t, size_t stSize)
  { // (Re)allocate if memory needed and return
    if(stSize) return UtilMemReAlloc<void>(vpAddr, stSize);
    // Zero for free memory
    UtilMemFree(vpAddr);
    // Return nothing
    return nullptr;
  }
  /* -- Warning callback --------------------------------------------------- */
  static void WarningCallback(void*const, const char*const cpMsg, int)
    { cLog->LogWarningExSafe("(Lua) $", cpMsg); }
  /* -- Lua end execution helper ------------------------------------------- */
  bool TryEventOrForce(const EvtMainCmd emcCode)
  { // If exit event already processing?
    if(cEvtMain->ExitRequested())
    { // Log event
      cLog->LogWarningExSafe("Lua sending event $ with forced confirm exit!",
        emcCode);
      // Change or confirm exit reason
      cEvtMain->Add(emcCode);
      // Quit by force instead
      cEvtMain->Add(EMC_LUA_CONFIRM_EXIT);
      // Quit forced
      return true;
    } // End lua execution
    cEvtMain->Add(emcCode);
    // Quit requested normally
    return false;
  }
  /* -- ReInitialise LUA context ------------------------------------------- */
  bool ReInit(void)
  { // If exit event already processing? Ignore sending another event
    if(cEvtMain->ExitRequested()) return false;
    // Send the event
    cEvtMain->Add(EMC_LUA_REINIT);
    // Return success
    return true;
  }
  /* -- Initialise LUA context --------------------------------------------- */
  void Init(void)
  { // Class initialised
    if(lsState) XC("Lua sandbox already initialised!");
    // Report progress
    cLog->LogDebugSafe("Lua sandbox initialising...");
    // Create lua context and bail if failed. ONLY use malloc() because we
    // could sometimes interleave allocations with C++ STL and use of any other
    // allocator will cause issues.
    lsState = LuaPtr{ lua_newstate(LuaDefaultAllocator, this), lua_close };
    if(!lsState) XC("Failed to create Lua context!");
    // Set panic callback
    lua_atpanic(GetState(), LuaUtilException);
    // Set warning catcher
    lua_setwarnf(GetState(), WarningCallback, this);
    // Register callback events
    cEvtMain->RegisterEx(*this);
    // Report initialisation with version and some important variables
    cLog->LogDebugExSafe("Lua sandbox initialised...\n"
      "- Stack size minimum: $; Stack size maximum: $.\n"
      "- Maximum CFunction calls: $; Maximum UpValues: $.",
      LUA_MINSTACK, LUAI_MAXSTACK, LUAI_MAXCCALLS, MAXUPVAL);
  }
  /* -- Constructor -------------------------------------------------------- */
  Lua(void) :
    /* --------------------------------------------------------------------- */
    LuaFlags{ LF_NONE },               // No flags set
    EvtMain::RegVec{                   // Lua events
      { EMC_LUA_REDRAW,                // Redraw event data
          bind(&Lua::SendRedraw,       // - Redraw callback
            this, _1) },               // - This class
      { EMC_LUA_ASK_EXIT,              // Ask exit event data
          bind(&Lua::AskExit,          // - Ask exit callback
            this, _1) },               // - This class
    },                                 // End of redraw event data
    bExiting(false),                   // Not exiting
    iOperations(0),                    // No operations
    iStack(0),                         // No stack
    iGCPause(0),                       // No GC pause
    iGCStep(0),                        // No GC step
    liSeed(0),                         // Random seed
    lrMainTick{ "MainTick" },          // Main tick event
    lrMainEnd{ "EndTick" },            // End tick event
    lrMainRedraw{ "OnRedraw" }         // Redraw event
    /* --------------------------------------------------------------------- */
    { }                                // No code
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Lua, DeInit())
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Lua)                 // Do not need defaults
  /* -- End ---------------------------------------------------------------- */
} *cLua = nullptr;                     // Pointer to static class
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
