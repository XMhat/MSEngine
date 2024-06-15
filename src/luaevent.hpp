/* == LUAEVENT.HPP ========================================================= **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## This helps classes have an event system which can fire events off   ## **
** ## to the event system from another thread and passed back to a lua    ## **
** ## callback on the main thread.                                        ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ILuaEvt {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ILuaRef::P;            using namespace ILuaUtil::P;
using namespace IError::P;             using namespace IEvtCore::P;
using namespace IEvtMain::P;           using namespace ILog::P;
using namespace IStd::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Private typedefs ----------------------------------------------------- */
// We need to collect the event ids of the events we dispatch so we can
// remove these events when the class is destroyed, or events that reference
// derived classes which have been destroyed will crash the engine.
typedef deque<EvtMain::QueueConstIt> LuaEvtsList;
/* == Class type for storing an event iterator and removing it ============= */
class LuaEvts :
  /* -- Initialisers ------------------------------------------------------- */
  private LuaEvtsList,                 // list of added event iterators
  protected mutex                      // To serialise access to the list
{ /* -- Return mutex ------------------------------------------------------- */
  mutex &LuaEvtsGetMutex(void) { return *this; }
  /* -- Remove iterator by id -------------------------------------- */ public:
  void LuaEvtsRemoveIterator(const size_t stId)
  { // Lock access to the list
    const LockGuard lgLuaEvtsSync{ LuaEvtsGetMutex() };
    // Throw error if the id is invalid
    if(stId >= size())
      XC("Invalid event index to remove!",
         "Requested", stId, "Maximum", size());
    // Clear stored event and remove all empty events from the end
    at(stId) = cEvtMain->Last();
    while(!empty() && back() == cEvtMain->Last()) pop_back();
  }
  /* -- Check if enough parameters ----------------------------------------- */
  template<size_t stMinimum>
    bool LuaEvtsCheckParams(const EvtMain::Params &empArgs)
  { // Minimum arguments must be two or more
    static_assert(stMinimum >= 2, "Must specify two parameters or more!");
    // If we have at least two parameters remove the iterator
    if(empArgs.size() >= 2) LuaEvtsRemoveIterator(empArgs[1].z);
    // Return true if required args is 2 because we've already checked that
    if constexpr(stMinimum == 2) return true;
    // Return success if we have enough parameters
    return empArgs.size() >= stMinimum;
  }
  /* -- Add a new event and stab iterator ---------------------------------- */
  template<typename ...VarArgs>void LuaEvtsDispatch(const EvtMainCmd emcCmd,
    const void*const vpClass, const VarArgs &...vaArgs)
  { // Iterator to return
    EvtMain::QueueConstIt qciItem;
    // Lock access to the list
    const LockGuard lgLuaEvtsSync{ LuaEvtsGetMutex() };
    // Current parameters list for event
    EvtMain::Params empArgs;
    // Reserve memory for parameters
    empArgs.reserve(sizeof...(VarArgs));
    // Create a new params list with the class and the events list size
    cEvtMain->AddExParam(emcCmd, qciItem, empArgs, vpClass, size(), vaArgs...);
    // Insert the iterator the new event.
    emplace_back(StdMove(qciItem));
  }
  /* -- Deinit event store-------------------------------------------------- */
  void LuaEvtsDeInit(void)
  { // Lock access to the list
    const LockGuard lgLuaEvtsSync{ LuaEvtsGetMutex() };
    // Done if no events lingering
    if(empty()) return;
    // Remove any queued events dispatched by this class as if they fire, the
    // app will crash because the class may have been destroyed.
    while(!empty())
    { // Get interator to the stored iterators
      const LuaEvtsList::const_iterator lelciIt{ cbegin() };
      // Get iterator to the event iterator and remove it if valid
      const EvtMain::QueueConstIt qciIt{ *lelciIt };
      if(qciIt != cEvtMain->Last()) cEvtMain->Remove(qciIt);
      // Erase the queue iterator from the queue
      erase(lelciIt);
    }
  }
  /* -- Constructor -------------------------------------------------------- */
  LuaEvts(void) { }
  /* -- Destructor --------------------------------------------------------- */
  ~LuaEvts(void) { LuaEvtsDeInit(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(LuaEvts)             // Disable copy operator and constructor
};/* ----------------------------------------------------------------------- */
/* == Class type for master class (send parameters on event trigger) ======= */
template<class MemberType>struct LuaEvtTypeParam
{ /* -- Send event to thiscall with no parameters -------------------------- */
  static void OnEvent(const EvtMain::Cell &emcArgs)
  { // Get reference to parameters and if there are no parameters?
    const EvtMain::Params &empArgs = emcArgs.vParams;
    if(empArgs.empty())
    { // Not enough parameters so show error in log
      cLog->LogErrorExSafe("LuaEvt got generic event $ with zero params!",
        emcArgs.evtCommand);
      // Done
      return;
    } // Get pointer to class and call if if it is valid
    MemberType*const mtPtr = reinterpret_cast<MemberType*>(empArgs.front().vp);
    if(mtPtr) return mtPtr->LuaEvtCallbackParam(emcArgs);
    // Show error so show error in log
    cLog->LogErrorExSafe(
      "LuaEvt got generic event $ with null class ptr from $ params!",
        emcArgs.evtCommand, empArgs.size());
  }
  /* -- Constructor (not interested) --------------------------------------- */
  LuaEvtTypeParam(void) { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(LuaEvtTypeParam)     // Remove Assignment operator/ctor
};/* ----------------------------------------------------------------------- */
/* == Class type for master class (send no parameters on event trigger) ==== */
template<class MemberType>struct LuaEvtTypeAsync // Used in async class
{ /* -- Send event to thiscall with no parameters -------------------------- */
  static void OnEvent(const EvtMain::Cell &emcArgs)
  { // Get reference to parameters and if we don't have any?
    const EvtMain::Params &empArgs = emcArgs.vParams;
    if(empArgs.empty())
    { // Not enough parameters so show error in log
      cLog->LogErrorExSafe("LuaEvt got async event $ with zero params!",
        emcArgs.evtCommand);
      // Done
      return;
    } // Get pointer to class and call if if it is valid
    if(MemberType*const mtPtr =
      reinterpret_cast<MemberType*>(empArgs.front().vp))
        return mtPtr->LuaEvtCallbackAsync(emcArgs);
    // Show error so show error in log
    cLog->LogErrorExSafe(
      "LuaEvt got async event $ with null class ptr from $ params!",
        emcArgs.evtCommand, empArgs.size());
  }
  /* -- Constructor (not interested) --------------------------------------- */
  LuaEvtTypeAsync(void) { }
  /* -- Delete defaults ---------------------------------------------------- */
  DELETECOPYCTORS(LuaEvtTypeAsync)     // Remove Assignment operator/ctor
};/* ----------------------------------------------------------------------- */
/* == Class for master class =============================================== */
template<class MemberType,             // Member object type
         class EventType>              // Event type object
class LuaEvtMaster :
  /* -- Base classes ------------------------------------------------------- */
  private EventType                    // Event type class
{ /* -- Private variables -------------------------------------------------- */
  const EvtMainCmd emcCmd;             // Event command
  /* -- Register the event ----------------------------------------- */ public:
  explicit LuaEvtMaster(const EvtMainCmd emcNCmd) :
    /* -- Initialisers ----------------------------------------------------- */
    emcCmd(emcNCmd)                    // Set the event id
    /* -- Register the event ----------------------------------------------- */
    { cEvtMain->Register(emcCmd, this->OnEvent); }
  /* -- Unregister the event ----------------------------------------------- */
  ~LuaEvtMaster(void) { cEvtMain->Unregister(emcCmd); }
  /* -- Delete defaults ---------------------------------------------------- */
  DELETECOPYCTORS(LuaEvtMaster)        // Remove Assignment operator/ctor
};/* == Routines for a collectors child class ============================== */
template<class MemberType,             // Member object type
         size_t stRefs=1>              // Number of references to store
class LuaEvtSlave :
  /* -- Base classes ------------------------------------------------------- */
  public LuaEvts,                      // Events handler
  public LuaRef<stRefs>                // Lua raw reference class
{ /* -- Private variables -------------------------------------------------- */
  EvtMainCmd       emcCmd;             // Event associated with this
  MemberType*const mtPtr;              // Parent of this class
  /* -- Event dispatch --------------------------------------------- */ public:
  template<typename ...VarArgs>void LuaEvtDispatch(const VarArgs &...vaArgs)
  { // Return if no callback is set. No point firing an event!
    if(!this->LuaRefIsSet()) return;
    // Insert a new event. We need to store the id of the iterator too so we
    // can remove it when we process the event in LuaEvtCallbackParam().
    LuaEvtsDispatch(emcCmd, reinterpret_cast<void*>(mtPtr), vaArgs...);
  }
  /* -- Event callback on main thread -------------------------------------- */
  void LuaEvtCallbackParam(const EvtMain::Cell &emcArgs) try
  { // Sanity check get number of mandatory parameters
    const size_t stMandatory = 2;
    // Get number of parameters and make sure we have enough parameters
    size_t stMax = emcArgs.vParams.size();
    if(stMax < stMandatory)
      XC("Not enough parameters to generic Lua event callback!",
         "Identifier", mtPtr->IdentGet(), "Event",   emcArgs.evtCommand,
         "Count",      stMax,             "Maximum", stMandatory);
    // Remove iterator from our events dispatched list
    LuaEvtsRemoveIterator(static_cast<size_t>(emcArgs.vParams[1].ui));
    // Lua is paused?
    if(uiLuaPaused)
    { // Show error in log
      cLog->LogWarningExSafe(
        "LuaEvt for generic event $ for '$' ignored because lua is paused!",
        emcArgs.evtCommand,  mtPtr->IdentGet(), this->LuaRefStateIsSet(),
        this->LuaRefGetId(), stMax);
      // Just return
      return;
    } // Check to see if we can write the function and it's parameters, if not?
    if(!LuaUtilIsStackAvail(this->LuaRefGetState(), stMax - 1))
      XC("Not enough stack space or memory, or param count overflowed!",
         "Identifier", mtPtr->IdentGet(),        "Event", emcArgs.evtCommand,
         "HaveState",  this->LuaRefStateIsSet(), "Ref",   this->LuaRefGetId(),
         "Params",     stMax);
    // Not have function?
    if(!this->LuaRefGetFunc())
      XC("Could not get callback function!",
         "Identifier", mtPtr->IdentGet(),        "Event", emcArgs.evtCommand,
         "HaveState",  this->LuaRefStateIsSet(), "Ref",   this->LuaRefGetId(),
         "Params",     stMax);
    // Enumerate add the rest of the parameters
    for(size_t stIndex = 2; stIndex < stMax; ++stIndex)
    { // Using event core namespace
      using namespace IEvtCore;
      const MVar &mvParam = emcArgs.vParams[stIndex];
      // Compare type
      switch(mvParam.t)
      { // Boolean?
        case MVT_BOOL:
          LuaUtilPushBool(this->LuaRefGetState(), mvParam.b);
          break;
        // C-String?
        case MVT_CSTR:
          LuaUtilPushCStr(this->LuaRefGetState(), mvParam.cp);
          break;
        // STL String?
        case MVT_STR:
          LuaUtilPushStr(this->LuaRefGetState(), *mvParam.str);
          break;
        // Float?
        case MVT_FLOAT:
          LuaUtilPushNum(this->LuaRefGetState(),
            static_cast<lua_Number>(mvParam.f));
          break;
        // Double?
        case MVT_DOUBLE:
          LuaUtilPushNum(this->LuaRefGetState(),
            static_cast<lua_Number>(mvParam.d));
          break;
        // Signed or unsigned integer?
        case MVT_UINT: [[fallthrough]]; case MVT_INT:
          LuaUtilPushInt(this->LuaRefGetState(),
            static_cast<lua_Integer>(mvParam.i));
          break;
        // Signed or unsigned long long?
        case MVT_ULONGLONG: [[fallthrough]]; case MVT_LONGLONG:
          LuaUtilPushInt(this->LuaRefGetState(),
            static_cast<lua_Integer>(mvParam.ll));
          break;
        // Signed or unsigned long int?
        case MVT_LONGUINT: [[fallthrough]]; case MVT_LONGINT:
          LuaUtilPushInt(this->LuaRefGetState(),
            static_cast<lua_Integer>(mvParam.li));
          break;
        // Unsupported type? Push nil
        default: [[fallthrough]]; case MVT_MAX:
          LuaUtilPushNil(this->LuaRefGetState());
          break;
      }
    } // Call the callback function.
    LuaUtilCallFuncEx(this->LuaRefGetState(),
      static_cast<int>(stMax - stMandatory));
  } // Exception occured? Disable lua callback and rethrow
  catch(const exception&) { this->LuaRefDeInit(); throw; }
  /* ----------------------------------------------------------------------- */
  void LuaEvtDeInit(void)
  { // De-initialise reference state
    this->LuaRefDeInit();
    // De-init event store
    LuaEvtsDeInit();
  }
  /* -- Initialise new variables ------------------------------------------- */
  void LuaEvtInitEx(lua_State*const lS)
  { // Initialise reference and if failed
    if(!this->LuaRefInit(lS))
      XC("Failed to reference variables!",
        "Identifier", mtPtr->IdentGet(), "Count", stRefs);
  }
  /* -- Initialise new variables ------------------------------------------- */
  void LuaEvtInitEx(lua_State*const lS, const size_t stId)
  { // Check that the sent state is the same one we have stored
    if(this->LuaRefGetState() != lS)
      XC("Cannot use callback from a different state to the one currently "
         "stored as the internal storage only supports storing one state "
         "for multiple references!",
        "StoredState", this->LuaRefStateIsSet(), "SentState", lS != nullptr,
        "Identifier",  mtPtr->IdentGet(),        "Reference", stId,
        "Count",       stRefs);
    // Initialise reference and if failed
    if(!this->LuaRefInitEx(stId))
      XC("Failed to reference new variable!",
        "Identifier", mtPtr->IdentGet(), "Reference", stId, "Count", stRefs);
  }
  /* -- Initialise with empty function ------------------------------------- */
  void LuaEvtInit(lua_State*const lS)
  { // Need 2 parameters and a function
    LuaUtilCheckParams(lS, 2);
    LuaUtilCheckFunc(lS, 2, "Callback");
    // Done
    LuaEvtInitEx(lS);
  }
  /* -- Standby constructor ------------------------------------------------ */
  LuaEvtSlave(MemberType*const mtNPtr, const EvtMainCmd emcNCmd) :
    /* -- Initialisers ----------------------------------------------------- */
    emcCmd(emcNCmd),                   // Command event id
    mtPtr(mtNPtr)                      // Set pointer to member object
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor to clean up any leftover events and references ---------- */
  ~LuaEvtSlave(void) { LuaEvtDeInit(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(LuaEvtSlave)         // Disable copy constructor
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
