/* == ASYNC.HPP ============================================================ */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Helps with loading data asynchronously and handle parallelisation.  ## */
/* ## Do not use class directly. It must be derived with another class.   ## */
/* ######################################################################### */
/* ## This class requires the parent class to have these functions...     ## */
/* ##   void AsyncReady(FileMap &fileClass)                               ## */
/* ##     - Fired when file is loading from memory (data block)           ## */
/* ######################################################################### */
/* ## It also requires the parent class' collector class to also register ## */
/* ## the supplied event command argument prior to using. The event must  ## */
/* ## point to the EventCallback function in this class.                  ## */
/* ######################################################################### */
/* ## Also do swap members in this class in anyway, it causes corruption! ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Prototypes ----------------------------------------------------------- */
namespace IfAsset {                    // Start of Asset module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfFileMap;             // Using filemap namespace
/* -- Prototypes ----------------------------------------------------------- */
static FileMap AssetExtract(const string&);      // Extract files from archives
static FileMap AssetLoadFromDisk(const string&); // Load file from disk
static size_t AssetGetPipeBufferSize(void);      // Get pipe buffer size
/* ------------------------------------------------------------------------- */
};                                     // End of asset namespace
/* ------------------------------------------------------------------------- */
namespace IfASync {                    // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfAsset;               // Using asset namespace
using namespace IfLuaEvt;              // Using luaevent namespace
/* ------------------------------------------------------------------------- */
enum ASyncProgressCommand              // For lua callback events
{ /* ----------------------------------------------------------------------- */
  APC_EXECSTART,                       // Exec started (2=Pid)
  APC_EXECDATAWRITE,                   // Exec writing data (2=Pos,3=Total)
  APC_FILESTART,                       // File opened (2=Siz,3=MTime,4=CTime)
};/* ----------------------------------------------------------------------- */
template<typename MemberType>class AsyncLoader :
  /* -- Base classes ------------------------------------------------------- */
  public Ident                         // Name of object (maybe repeated)
{ /* -- Private typedefs ------------------------------------------ */ private:
  enum AsyncResult                     // Async loading results
  { /* --------------------------------------------------------------------- */
    AR_STANDBY,                        // Loading has not started
    AR_LOADING,                        // Resource is loading
    AR_SUCCESS,                        // Loading succeeded
    AR_SUCCESS_PARAM,                  // Loading succeeded (param supplied)
    AR_ABORT,                          // Loading aborted
    AR_ERROR,                          // Loading failed
    AR_UNKNOWN                         // Unknown code
  }; /* -------------------------------------------------------------------- */
  enum ASyncCmdType                    // Loader content type
  { /* --------------------------------------------------------------------- */
    BA_NONE,                           // No event
    BA_FILE,                           // Load from file
    BA_MEMORY,                         // Load from memory
    BA_EXECUTE                         // Execute command-line
  }; /* -------------------------------------------------------------------- */
  enum ASyncCallback                   // Loader content type
  { /* --------------------------------------------------------------------- */
    LR_CLASS,                          // [0] Ref to the pointer to class
    LR_SUCCESS,                        // [1] Ref to the success callback
    LR_PROGRESS,                       // [2] Ref to the progress callback
    LR_ERROR,                          // [3] Ref to the error callback
    LR_MAX                             // [4] Maximum lua references
  };/* --------------------------------------------------------------------- */
  typedef LuaEvtSlave<MemberType,LR_MAX> LuaEvtCallback;
  /* -- Initialised in parent class ---------------------------------------- */
  MemberType      &cAsyncOwner;        // Parent collector that owns this
  EvtMainCmd       evtAsyncEvent;      // Event command to fire on load
  /* -- Private variables -------------------------------------------------- */
  LuaEvtCallback   lesAsync;           // Async state and references
  Thread           tAsyncThread;       // Asynchronous loading
  string           strAsyncError;      // The last error exception
  Memory           mAsyncLoadData;     // Loading from memory data
  ASyncCmdType     aiAsyncType;        // Async load init type
  SafeUInt         uiAsyncPid;         // Pid of executing process
  /* -- Dispatch event without parameter ----------------------------------- */
  void AsyncCompletionDispatch(const AsyncResult arResult)
  { // Signal events handler to execute event callback on the next frame.
    // Add the event to the event store so we can remove the event when the
    // destructor is called before the event is called.
    lesAsync.LuaEvtsDispatch(evtAsyncEvent,
      reinterpret_cast<void*>(&cAsyncOwner), arResult);
  }
  /* -- Send completion ---------------------------------------------------- */
  void AsyncCompletion(void)
  { // Dispatch the result of the asynchronous event. Send a cancel instead if
    // the thread wants to terminate.
    AsyncCompletionDispatch(tAsyncThread.ThreadShouldExit() ?
      AR_ABORT : AR_SUCCESS);
  }
  /* -- Send completion ---------------------------------------------------- */
  void AsyncCompletionWithParam(const int64_t qwParam)
  { // If thread was asked to abort? Dispatch the asynchronous event and return
    if(tAsyncThread.ThreadShouldExit())
      return AsyncCompletionDispatch(AR_ABORT);
    // Dispatch the asynchronous event with parameter. Unforunately we have to
    // cast any value we get into int64 because we read as int64 at the event
    // handling routine, unless we send a type as well which probably isn't
    // worth the effort!
    lesAsync.LuaEvtsDispatch(evtAsyncEvent,
      reinterpret_cast<void*>(&cAsyncOwner), AR_SUCCESS_PARAM, qwParam);
  }
  /* -- Send progress event ------------------------------------------------ */
  template<typename... V>
    void AsyncProgress(const ASyncProgressCommand apcCmd, V... vVars)
      { lesAsync.LuaEvtsDispatch(evtAsyncEvent,
          reinterpret_cast<void*>(&cAsyncOwner), AR_LOADING,
          static_cast<uint64_t>(apcCmd), vVars...); }
  /* -- Write to pipe assistant -------------------------------------------- */
  void AsyncWriteToPipe(SysBase::SysPipe &spProcess, size_t &stPosition,
    const size_t stBuffer)
  { // Write a block of data and get how much we wrote. We hard fail if we did
    // not write the correct number of bytes
    const size_t stWrote =
      spProcess.Send(mAsyncLoadData.Read(stPosition), stBuffer);
    if(stWrote != stBuffer)
      XCS("Not enough data written to process pipe!",
        "Ident",    spProcess.IdentGet(), "Pid",    spProcess.GetPid(),
        "Expected", stBuffer,          "Actual", stWrote,
        "Position", stPosition);
    // Move forward
    stPosition += stBuffer;
    // Send progress update
    AsyncProgress(APC_EXECDATAWRITE,
      static_cast<uint64_t>(stPosition),
      static_cast<uint64_t>(mAsyncLoadData.Size()));
  }
  /* -- Init from file synchronously ---------------------------- */ protected:
  void SyncInitFile(const string &strFilename)
  { // Set filename
    IdentSet(strFilename);
    // Load from specified file
    FileMap fmData{ AssetLoadFromDisk(strFilename) };
    // Send to derived class and register
    SyncLoadDataAndRegister(fmData);
  }
  /* -- Execute function (returns exit code) ----------------------- */ public:
  int64_t AsyncExecute(void)
  { // Duration of execution
    const ClockChrono<> ccExecute;
    // Open pipe to application
    SysBase::SysPipe spProcess{ IdentGet() };
    // Record pid. This is so the destructor can kill it on exit
    uiAsyncPid = spProcess.GetPid();
    // Send progress event
    AsyncProgress(APC_EXECSTART, static_cast<uint64_t>(uiAsyncPid.load()));
    // If we have data to send?
    if(mAsyncLoadData.NotEmpty())
    { // Current position
      size_t stPosition = 0;
      // Buffer size to write (maybe configurable todo)
      const size_t stBuffer = AssetGetPipeBufferSize(),
        // Get amount of extra data to write
        stExtra = mAsyncLoadData.Size() % stBuffer,
        // Clamp end position to number of pages
        stEnd = mAsyncLoadData.Size() < stBuffer ?
          mAsyncLoadData.Size() : mAsyncLoadData.Size() - stExtra;
      // If we are writing more than one page? Copy full pages of buffers
      // until we are left with remainders.
      if(stExtra > stBuffer)
        while(stPosition < stEnd && tAsyncThread.ThreadShouldNotExit())
          AsyncWriteToPipe(spProcess, stPosition, stBuffer);
      // If theres remainder to write then write that too.
      if(stExtra) AsyncWriteToPipe(spProcess, stPosition, stExtra);
      // Close the writing portion of the pipe
      spProcess.SendFinish();
      // Report that we write data
      cLog->LogInfoExSafe("Async pipe wrote $ bytes to stdin of pid $ in $!",
        stPosition, spProcess.GetPid(),
        ToShortDuration(ccExecute.CCDeltaToDouble()));
      // Clear the memory to re-use for output
      mAsyncLoadData.DeInit();
    } // List of output
    MemoryList mlBlocks;
    size_t stTotalRead = 0;
    // Until the thread says we should exit
    while(tAsyncThread.ThreadShouldNotExit())
    { // Read process output and break out of loop if nothing read
      Memory mBuffer{ spProcess.ReadBlock() };
      if(mBuffer.Empty()) break;
      // Increase total bytes read
      stTotalRead += mBuffer.Size();
      // Move it into the packet list
      mlBlocks.emplace_back(StdMove(mBuffer));
    } // Finished with pipe. Also gets the exit code too.
    spProcess.Finish();
    // Pid no longer valid
    uiAsyncPid = 0;
    // Log pipe result.
    cLog->LogInfoExSafe(
      "Async pipe returned $ bytes and exit code $ from pid $ in $!",
      stTotalRead, spProcess.SysPipeBaseGetStatus(), spProcess.GetPid(),
      ToShortDuration(ccExecute.CCDeltaToDouble()));
    // If we have no memory stored? Return exit code
    if(!stTotalRead) return spProcess.SysPipeBaseGetStatus();
    // Initialise memory for program output data block
    mAsyncLoadData.InitBlank(stTotalRead);
    // Byte offset counter
    size_t stOffset = 0;
    // Loop until...
    do
    { // Get packet memory block and copy it into our dest memory block
      const DataConst &dcPacket = mlBlocks.front();
      mAsyncLoadData.WriteBlock(stOffset, dcPacket);
      // Increment counter
      stOffset += dcPacket.Size();
      // Pop packet
      mlBlocks.pop_front();
    } // ...list is fully emptied
    while(!mlBlocks.empty());
    // Return exit code
    return spProcess.SysPipeBaseGetStatus();
  }
  /* -- Process a file map and dispatch events ----------------------------- */
  void AsyncParseFileMap(FileMap &fmFile)
  { // Send progress event with the file size
    AsyncProgress(APC_FILESTART,
      static_cast<uint64_t>(fmFile.Size()),
      static_cast<uint64_t>(fmFile.FileMapModifiedTime()),
      static_cast<uint64_t>(fmFile.FileMapCreationTime()));
    // Move our memory block into a file map and send it to loader
    cAsyncOwner.AsyncReady(fmFile);
  }
  /* -- Move the stored memory block into a new filemap and parse it ------- */
  void AsyncParseMemory(void)
    { FileMap fmFile{ IdentGet(),
        StdMove(mAsyncLoadData), cmSys.GetTimeS() };
      AsyncParseFileMap(fmFile); }
  /* -- Load the specified file and parse it ------------------------------- */
  void AsyncParseFile(void)
    { FileMap fmFile{ AssetExtract(IdentGet()) };
      AsyncParseFileMap(fmFile); }
  /* -- Async off-main thread function ------------------------------------- */
  int AsyncThreadMain(Thread&)
  { // Capture exceptions. Remember that after these operations the memory
    // at mAsyncLoadData should be de-initialised. So don't use it again.
    try
    { // How are we loading?
      switch(aiAsyncType)
      {  // Do nothing?
        case BA_NONE:
        { // Just send a blank file and signal completion
          FileMap fmData;
          cAsyncOwner.AsyncReady(fmData);
          AsyncCompletion();
          break;
        } // Loading from a file?
        case BA_FILE:
        { // Open and load the file and signal completion
          AsyncParseFile();
          AsyncCompletion();
          break;
        } // Execute as command line?
        case BA_EXECUTE:
        { // Perform command line execution and send return code to handler
          const int64_t qwResult = AsyncExecute();
          AsyncParseMemory();
          AsyncCompletionWithParam(qwResult);
          break;
        } // Load from memory?
        case BA_MEMORY:
        { // Parse the memory block and signal completion
          AsyncParseMemory();
          AsyncCompletion();
          break;
        }
      } // Return success to thread manager
      return 1;
    } // exception occured?
    catch(const exception &E)
    { // Prepend the reason incase there are nested exceptions
      strAsyncError.insert(0, E.what());
      // Signal completion
      AsyncCompletionDispatch(AR_ERROR);
      // Report error in log
      cLog->LogErrorExSafe("(ASYNCLOADER THREAD EXCEPTION) $", strAsyncError);
      // Return failure to thread manager
      return -1;
    }
  }
  /* ----------------------------------------------------------------------- */
  void AsyncInit(lua_State*const lsS, const string &strName,
    const string &strL)
  { // Check we got a valid lua state and that theres something in the array
    if(!lsS) XC("Invalid state!");
    // Set the filename
    IdentSet(strName);
    // Parse the class, error and success functions.
    lesAsync.LuaEvtInitEx(lsS);
    // Save the current stack because if an error occurs asynchronously, the
    // event subsystem executes the callback and will be empty.
    strAsyncError = StdMove(GetStack(lsS));
    // Begin async thread
    tAsyncThread.ThreadInit(Append(strL, ':', strName),
      bind(&AsyncLoader<MemberType>::AsyncThreadMain,
        this, _1), this);
  }
  /* ----------------------------------------------------------------------- */
  void AsyncStop(void)
  { // If a pid is set then we need to kill it
    if(uiAsyncPid) { cSystem->TerminatePid(uiAsyncPid); uiAsyncPid = 0; }
    // Wait for thread to terminate
    tAsyncThread.ThreadDeInit();
  }
  /* -- Async do protected dispatch (assumes params already on lua stack) -- */
  void AsyncDoLuaThrowErrorHandler(const EvtMain::Cell &epData)
  { // Get error function callback
    if(lesAsync.LuaRefGetFunc(LR_ERROR))
    { // Push the error message
      PushCppString(lesAsync.LuaRefGetState(), strAsyncError);
      // The memory for the error is no longer needed
      strAsyncError.clear();
      strAsyncError.shrink_to_fit();
      // Wait for thread and register the class
      AsyncStopAndRegister();
      // Now do the callback. An exception could occur here.
      CallFuncEx(lesAsync.LuaRefGetState(), 1);
    } // Invalid userdata?
    else cLog->LogErrorExSafe("AsyncLoader got invalid params in failure "
      "event $ for '$' with luastate($) and fref($) from $ params!",
      epData.evtCommand, IdentGet(), lesAsync.LuaRefStateIsSet(),
      lesAsync.LuaRefGetFunc(LR_ERROR), epData.vParams.size());
  }
  /* -- Async do protected dispatch (assumes params already on lua stack) -- */
  void AsyncDoLuaProtectedDispatch(const EvtMain::Cell &epData,
    const int iParams, const int iHandler)
  { // Compare error code
    switch(PCallExSafe(lesAsync.LuaRefGetState(), iParams, 0, iHandler))
    { // No error so remove error handler value and return
      case 0: return;
      // Run-time error
      case LUA_ERRRUN:
        strAsyncError.insert(0, Append("Async runtime error! > ",
          GetAndPopString(lesAsync.LuaRefGetState()))); break;
      // Memory allocation error
      case LUA_ERRMEM:
        strAsyncError.insert(0, "Async memory allocation error!"); break;
      // Error + error in error handler
      case LUA_ERRERR:
        strAsyncError.insert(0, "Error in async error handler!"); break;
      // Unknown error
      default:
        strAsyncError.insert(0, "Unknown error in async callback!"); break;
    } // Throw error back to user error handler
    AsyncDoLuaThrowErrorHandler(epData);
  }
  /* -- Waits for the thread to finish and registers the class ------------- */
  void AsyncStopAndRegister(void)
  { // Wait for the thread to terminate if it is still running
    AsyncStop();
    // Register the class
    cAsyncOwner.CollectorRegister();
  }
  /* -- Async do protected call dispatams already pushed onto lua stack) --- */
  void AsyncDoFinishLuaProtectedDispatch(const EvtMain::Cell &epData,
    const int iParam, const int iHandler)
  { // Wait for thread and register the class
    AsyncStopAndRegister();
    // Now do the callback. An exception could occur here as well and it
    // should be protected.
    AsyncDoLuaProtectedDispatch(epData, iParam, iHandler);
  }
  /* --------------------------------------------------------------- */ public:
  void AsyncWait(void) { tAsyncThread.ThreadWait(); }
  /* ----------------------------------------------------------------------- */
  void AsyncCancel(void)
  { // Wait for the thread to stop
    AsyncStop();
    // Unreference the class
    lesAsync.LuaEvtDeInit();
  }
  /* -- The thread only calls LoadData() and thats it ---------------------- */
  void AsyncInitNone(lua_State*const lsS, const string &strName,
    const string &strLabel)
  { // Loading from memory
    aiAsyncType = BA_NONE;
    // Init rest of members
    AsyncInit(lsS, strName, strLabel);
  }
  /* -- The thread loads from a filemap and calls LoadData() --------------- */
  void AsyncInitFile(lua_State*const lsS, const string &strFilename,
    const string &strLabel)
  { // Loading from file
    aiAsyncType = BA_FILE;
    // Init rest of members
    AsyncInit(lsS, strFilename, strLabel);
  }
  /* -- The thread loads from the command-line ----------------------------- */
  void AsyncInitCmdLine(lua_State*const lsS, const string &strCmdLine,
    const string &strLabel, Memory &mbInput)
  { // Loading from command-line
    aiAsyncType = BA_EXECUTE;
    // Set memory sent to stdin
    mAsyncLoadData = StdMove(mbInput);
    // Init rest of members
    AsyncInit(lsS, strCmdLine, strLabel);
  }
  /* -- The thread loads from existing memory and calls LoadData() --------- */
  void AsyncInitArray(lua_State*const lsS, const string &strName,
    const string &strLabel, Memory &&mData)
  { // Set data to load from
    mAsyncLoadData = StdMove(mData);
    // Loading from memory
    aiAsyncType = BA_MEMORY;
    // Init rest of members
    AsyncInit(lsS, strName, strLabel);
  }
  /* -- Send load data update and register in collector -------------------- */
  void SyncLoadDataAndRegister(FileMap &fmData)
  { // Send file map to derived class
    cAsyncOwner.AsyncReady(fmData);
    // Load succeeded so register the block.
    cAsyncOwner.CollectorRegister();
  }
  /* -- Init from array synchronously -------------------------------------- */
  void SyncInitArray(const string &strName, Memory &&mData)
  { // Set identifier
    IdentSet(strName);
    // Put the memory block into a file map and load the block
    FileMap fmData{ strName, StdMove(mData), cmSys.GetTimeS() };
    // Send to derived class and register
    SyncLoadDataAndRegister(fmData);
  }
  /* -- Init from file synchronously with filename checking ---------------- */
  void SyncInitFileSafe(const string &strFilename)
  { // Set filename
    IdentSet(strFilename);
    // Load from specified file from archives or disk
    FileMap fmData{ AssetExtract(strFilename) };
    // Send to derived class and register
    SyncLoadDataAndRegister(fmData);
  }
  /* -- Async thread event callback (called by LuaEvtMaster) ------------- */
  void LuaEvtCallbackAsync(const EvtMain::Cell &epData) try
  { // Get reference to string vector and we need three parameters
    // [0]=Pointer to socket class, [1]=Event list id, [2]=Status
    const EvtMain::Params &eParams = epData.vParams;
    // Event result
    const AsyncResult uiAsyncResult =
      lesAsync.template LuaEvtsCheckParams<3>(eParams) ?
        static_cast<AsyncResult>(eParams[2].lui) : AR_UNKNOWN;
    // If lua is not paused?
    if(!bLuaPaused)
    { // Whats the code sent in the event?
      switch(uiAsyncResult)
      { // Unknown operation?
        case AR_UNKNOWN:
        { // Log the invalid parameter count
          cLog->LogWarningExSafe("AsyncLoader parameter mismatch of $ in "
            "event $ for '$' with status $!", eParams.size(),
            epData.evtCommand, IdentGet(), uiAsyncResult);
          // Done
          break;
        } // The operation is still loading?
        case AR_LOADING:
        { // Push and get error callback function id
          const int iErrorHandler =
            PushAndGetGenericErrorId(lesAsync.LuaRefGetState());
          // Push the progress callback and if succeeded?
          if(lesAsync.LuaRefGetFunc(LR_PROGRESS))
          { // Push the sent parameters onto the stack
            const size_t stMax = eParams.size();
            for(size_t stIndex = 3; stIndex < stMax; ++stIndex)
              PushInteger(lesAsync.LuaRefGetState(),
                eParams[stIndex].ll);
            // Execute the progress callback
            AsyncDoLuaProtectedDispatch(epData, static_cast<int>(stMax - 3),
              iErrorHandler);
            // Do not do anything else
            return;
          } // Show error
          else cLog->LogErrorExSafe("AsyncLoader got invalid params in "
            "progress event $ for '$' with luastate($) and fref($) from $ "
            "params!",
            epData.evtCommand, IdentGet(), lesAsync.LuaRefStateIsSet(),
            lesAsync.LuaRefGetFunc(LR_PROGRESS), eParams.size());
          // Done
          break;
        } // The operation succeeded with a parameter attached?
        case AR_SUCCESS_PARAM:
        { // We need that third (first) parameter and if we do?
          if(eParams.size() >= 4)
          { // Push and get error callback function id
            const int iErrorHandler =
              PushAndGetGenericErrorId(lesAsync.LuaRefGetState());
            // Push the success callback and if succeeded?
            if(lesAsync.LuaRefGetFunc(LR_SUCCESS))
            { // Push the class ref and if both succeeded?
              if(lesAsync.LuaRefGetUData())
              { // Push the specified parameter
                PushInteger(lesAsync.LuaRefGetState(), eParams[3].ll);
                // Dispatch the event with two parameters
                AsyncDoFinishLuaProtectedDispatch(epData, 2, iErrorHandler);
              } // Failed? Write error to log
              else cLog->LogErrorExSafe("AsyncLoader got invalid userdata "
                "param in success param event $ for '$' from $ params!",
                epData.evtCommand, IdentGet(), eParams.size());
            } // Show error
            else cLog->LogErrorExSafe("AsyncLoader got invalid params in "
              "success param event $ for '$' with luastate($) and fref($) "
              "from $ params!",
              epData.evtCommand, IdentGet(), lesAsync.LuaRefStateIsSet(),
              lesAsync.LuaRefGetFunc(LR_SUCCESS), eParams.size());
          } // Log error
          else cLog->LogErrorExSafe("AsyncLoader got invalid value in "
            "success param event $ for '$' from $ params!", epData.evtCommand,
              IdentGet(), eParams.size());
          // Done
          break;
        } // The operation succeeded?
        case AR_SUCCESS:
        { // Push and get error callback function id
          const int iErrorHandler =
            PushAndGetGenericErrorId(lesAsync.LuaRefGetState());
          // Push the success callback and if succeeded?
          if(lesAsync.LuaRefGetFunc(LR_SUCCESS))
          { // Push the class ref and if both succeeded?
            if(lesAsync.LuaRefGetUData())
              AsyncDoFinishLuaProtectedDispatch(epData, 1, iErrorHandler);
            // Failed? Write error to log
            else cLog->LogErrorExSafe("AsyncLoader got invalid userdata param "
              "in success event $ for '$' from $ params!",
              epData.evtCommand, IdentGet(), eParams.size());
          } // Show error
          else cLog->LogErrorExSafe("AsyncLoader got invalid params in "
            "success event $ for '$' with luastate($) and fref($) from $ "
            "params!",
            epData.evtCommand, IdentGet(), lesAsync.LuaRefStateIsSet(),
            lesAsync.LuaRefGetFunc(LR_SUCCESS), eParams.size());
          // Done
          break;
        } // If there was an error?
        case AR_ERROR:
        { // Push the error callback and if succeeded?
          AsyncDoLuaThrowErrorHandler(epData);
          // Done
          break;
        } // Other error?
        default:
        { // Report it and fall through to clean-up
          cLog->LogErrorExSafe(
            "AsyncLoader got unknown result $ after loading '$'!",
            uiAsyncResult, IdentGet());
        } // If the thread was told to abort? Clear references and just return
        case AR_ABORT: break;
      }
    } // Clear references and state or Lua's GC won't ever delete the class
    lesAsync.LuaRefDeInit();
  } // Exception occured? Disable lua callback/refs and rethrow
  catch(const exception&) { lesAsync.LuaRefDeInit(); throw; }
  /* -- Main constructor --------------------------------------------------- */
  AsyncLoader(                         // Initialise with derived class
    MemberType*const cO,               // The pointer to the class
    const EvtMainCmd eC) :             // The event id to use
    /* -- Initialisers ----------------------------------------------------- */
    cAsyncOwner{ *cO },                // Initialise owner of this class
    evtAsyncEvent(eC),                 // Initialise event id
    lesAsync{ cO, eC },                // Initialise Lua event class
    tAsyncThread{ SysThread::Low },    // Initialise low priority thread
    aiAsyncType(BA_NONE),              // Initialise load source type
    uiAsyncPid(0)                      // Initialise pid for BA_EXECUTE
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* ----------------------------------------------------------------------- */
  // NOTE: This is here for convenience only. ALWAYS call AsyncCancel() in your
  // derived classes because static objects created in the event callback while
  // waiting for it to be called will NOT be cleaned up properly and cause
  // memory leaks.
  ~AsyncLoader(void)
  { // Ignore if thread isn't running
    if(tAsyncThread.ThreadIsNotRunning()) return;
    // Print a warning to say that we should not be allowing this destructor
    // the clean up the thread
    cLog->LogWarningExSafe("AsyncLoader waiting for '$' to unload in its own "
      "destructor. There could be memory leaks!", strAsyncError);
    // Wait for the thread to stop
    AsyncStop();
  }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(AsyncLoader)         // Disable copy constructor
};/* -- Function to wait for async of all members in a collector ----------- */
template<class MemberType,
         class LockType,
         class BaseType = CLHelper<MemberType, CLHelperSafe<MemberType>>,
         class EventType = LuaEvtMaster<MemberType,
           LuaEvtTypeAsync<MemberType>>>
class CLHelperAsync :
  /* -- Derivced classes --------------------------------------------------- */
  public BaseType                      // CLHelper
{ /* -- Functions -------------------------------------------------- */ public:
  const EventType  lemAsync;           // Event name id
  /* -- Functions ---------------------------------------------------------- */
  void WaitAsync(void)
  { // Lock the collector's mutex
    const LockGuard lgSynchronise{ this->CollectorGetMutex() };
    // Sync all members
    for(MemberType*const cChild : *this) cChild->AsyncWait();
  }
  /* -- Constructor -------------------------------------------------------- */
  CLHelperAsync(const char*const cpN, const EvtMainCmd eC) :
    /* -- Initialisers ----------------------------------------------------- */
    BaseType{ cpN },                   // Initialise name of base tpye
    lemAsync{ eC }                     // Initialise event name
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CLHelperAsync)       // Supress copy constructor for safety
};/* -- End of module namespace -------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */
