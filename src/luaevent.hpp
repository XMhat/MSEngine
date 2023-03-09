/* == LUAEVENT.HPP ========================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This helps classes have an event system which can fire events off   ## */
/* ## to the event system from another thread and passed back to a lua    ## */
/* ## callback on the main thread.                                        ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfLuaEvt {                   // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfEvtMain;             // Using event interface
using namespace IfLuaRef;              // Using luaref interface
/* -- Private typedefs ----------------------------------------------------- */
// We need to collect the event ids of the events we dispatch so we can
// remove these events when the class is destroyed, or events that reference
// derived classes which have been destroyed will crash the engine.
typedef deque<EvtMain::QueueConstIt> LuaEvtsList;
typedef LuaEvtsList::const_iterator LuaEvtsListIt;
/* == Class type for storing an event iterator and removing it ============= */
class LuaEvts :
  /* -- Initialisers ------------------------------------------------------- */
  private LuaEvtsList,                 // list of added event iterators
  protected mutex                      // To serialise access to the list
{ /* -- Return mutex ---------------------------------------------- */ private:
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
    bool LuaEvtsCheckParams(const EvtMain::Params &eParams)
  { // Minimum arguments must be two or more
    static_assert(stMinimum >= 2, "Must specify two parameters or more!");
    // If we have at least two parameters remove the iterator
    if(eParams.size() >= 2) LuaEvtsRemoveIterator(eParams[1].z);
    // Return true if required args is 2 because we've already checked that
    if constexpr(stMinimum == 2) return true;
    // Return success if we have enough parameters
    return eParams.size() >= stMinimum;
  }
  /* -- Add a new event and stab iterator ---------------------------------- */
  template<typename... V>void LuaEvtsDispatch(const EvtMainCmd &eCmd,
    const void*const vpClass, V... vVars)
  { // Iterator to return
    EvtMain::QueueConstIt qciItem;
    // Lock access to the list
    const LockGuard lgLuaEvtsSync{ LuaEvtsGetMutex() };
    // Current parameters list for event
    EvtMain::Params pData;
    // Reserve memory for parameters
    pData.reserve(sizeof...(V));
    // Create a new params list with the class and the events list size
    cEvtMain->AddExParam(eCmd, qciItem, pData, vpClass, size(), vVars...);
    // Insert the iterator the new event.
    emplace_back(move(qciItem));
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
      const LuaEvtsListIt lrIter{ cbegin() };
      // Get iterator to the event iterator and remove it if valid
      const EvtMain::QueueConstIt qIter{ *lrIter };
      if(qIter != cEvtMain->Last()) cEvtMain->Remove(qIter);
      // Erase the queue iterator from the queue
      erase(lrIter);
    }
  }
  /* -- Constructor -------------------------------------------------------- */
  LuaEvts(void) { }
  /* -- Destructor --------------------------------------------------------- */
  ~LuaEvts(void) { LuaEvtsDeInit(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(LuaEvts);            // Disable copy operator and constructor
};/* ----------------------------------------------------------------------- */
/* == Class type for master class (send parameters on event trigger) ===== */
template<class MemberType>class LuaEvtTypeParam
{ /* -- Send event to thiscall with no parameters ------------------ */ public:
  static void OnEvent(const EvtMain::Cell &eD)
  { // Get reference to parameters and if there are no parameters?
    const EvtMain::Params &vParams = eD.vParams;
    if(vParams.empty())
    { // Not enough parameters so show error in log
      LW(LH_ERROR, "LuaEvt got generic event $ with zero params!",
        eD.evtCommand);
      // Done
      return;
    } // Get pointer to class and call if if it is valid
    MemberType*const cPtr = reinterpret_cast<MemberType*>(vParams[0].vp);
    if(cPtr) return cPtr->LuaEvtCallbackParam(eD);
    // Show error so show error in log
    LW(LH_ERROR,
      "LuaEvt got generic event $ with null class ptr from $ params!",
        eD.evtCommand, vParams.size());
  }
  /* -- Constructor (not interested) --------------------------------------- */
  LuaEvtTypeParam(void) { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(LuaEvtTypeParam);    // Remove Assignment operator/ctor
};/* ----------------------------------------------------------------------- */
/* == Class type for master class (send no parameters on event trigger) ==== */
template<class MemberType>class LuaEvtTypeAsync // Used in async class
{ /* -- Send event to thiscall with no parameters ------------------ */ public:
  static void OnEvent(const EvtMain::Cell &eD)
  { // Get reference to parameters and if we don't have any?
    const EvtMain::Params &vParams = eD.vParams;
    if(vParams.empty())
    { // Not enough parameters so show error in log
      LW(LH_ERROR, "LuaEvt got async event $ with zero params!",
        eD.evtCommand);
      // Done
      return;
    } // Get pointer to class and call if if it is valid
    if(MemberType*const mtPtr = reinterpret_cast<MemberType*>(vParams[0].vp))
      return mtPtr->LuaEvtCallbackAsync(eD);
    // Show error so show error in log
    LW(LH_ERROR,
      "LuaEvt got async event $ with null class ptr from $ params!",
        eD.evtCommand, vParams.size());
  }
  /* -- Constructor (not interested) --------------------------------------- */
  LuaEvtTypeAsync(void) { }
  /* -- Delete defaults ---------------------------------------------------- */
  DELETECOPYCTORS(LuaEvtTypeAsync);    // Remove Assignment operator/ctor
};/* ----------------------------------------------------------------------- */
/* == Class for master class =============================================== */
template<class MemberType,             // Member object type
         class EventType>              // Event type object
class LuaEvtMaster :
  /* -- Derived classes ---------------------------------------------------- */
  private EventType                    // Event type class
{ /* -- Private variables ----------------------------------------- */ private:
  const EvtMainCmd evtCmd;             // Event command
  /* -- Register the event ----------------------------------------- */ public:
  explicit LuaEvtMaster(const EvtMainCmd eC) :
    /* -- Initialisers ----------------------------------------------------- */
    evtCmd(eC)                         // Set the event id
    /* -- Register the event ----------------------------------------------- */
    { cEvtMain->Register(evtCmd, this->OnEvent); }
  /* -- Unregister the event ----------------------------------------------- */
  ~LuaEvtMaster(void) { cEvtMain->Unregister(evtCmd); }
  /* -- Delete defaults ---------------------------------------------------- */
  DELETECOPYCTORS(LuaEvtMaster);       // Remove Assignment operator/ctor
};/* == Routines for a collectors child class ============================== */
template<class MemberType,             // Member object type
         size_t Refs=1>                // Number of references to store
class LuaEvtSlave :
  /* -- Base classes ------------------------------------------------------- */
  public LuaEvts,                      // Events handler
  public LuaRef<Refs>                  // Lua raw reference class
{ /* -- Private variables ----------------------------------------- */ private:
  EvtMainCmd       evtCmd;             // Event associated with this
  MemberType*const cMember;            // Parent of this class
  /* -- Event dispatch --------------------------------------------- */ public:
  template<typename...V>void LuaEvtDispatch(V... vVars)
  { // Return if no callback is set. No point firing an event!
    if(!this->LuaRefIsSet()) return;
    // Insert a new event. We need to store the id of the iterator too so we
    // can remove it when we process the event in LuaEvtCallbackParam().
    LuaEvtsDispatch(evtCmd, reinterpret_cast<void*>(cMember), vVars...);
  }
  /* -- Event callback on main thread -------------------------------------- */
  void LuaEvtCallbackParam(const EvtMain::Cell &epData) try
  { // Sanity check get number of mandatory parameters
    const size_t stMandatory = 2;
    // Get number of parameters and make sure we have enough parameters
    size_t stMax = epData.vParams.size();
    if(stMax < stMandatory)
      XC("Not enough parameters to generic Lua event callback!",
         "Identifier", cMember->IdentGet(), "Event", epData.evtCommand,
         "Count",      stMax,            "Maximum", stMandatory);
    // Remove iterator from our events dispatched list
    LuaEvtsRemoveIterator(static_cast<size_t>(epData.vParams[1].ui));
    // Lua is paused?
    if(bLuaPaused)
    { // Show error in log
      LW(LH_WARNING,
        "LuaEvt for generic event $ for '$' ignored because lua is paused!",
        epData.evtCommand,  cMember->IdentGet(), this->LuaRefStateIsSet(),
        this->LuaRefGetId(), stMax);
      // Just return
      return;
    } // Check to see if we can write the function and it's parameters, if not?
    if(!StackHasCapacity(this->LuaRefGetState(), stMax - 1))
      XC("Not enough stack space or memory, or param count overflowed!",
         "Identifier", cMember->IdentGet(),         "Event", epData.evtCommand,
         "HaveState",  this->LuaRefStateIsSet(), "Ref", this->LuaRefGetId(),
         "Params",     stMax);
    // Not have function?
    if(!this->LuaRefGetFunc())
      XC("Could not get callback function!",
         "Identifier", cMember->IdentGet(),         "Event", epData.evtCommand,
         "HaveState",  this->LuaRefStateIsSet(), "Ref", this->LuaRefGetId(),
         "Params",     stMax);
    // Enumerate add the rest of the parameters
    for(size_t stIndex = 2; stIndex < stMax; ++stIndex)
      PushInteger(this->LuaRefGetState(),
        static_cast<lua_Integer>(epData.vParams[stIndex].ui));
    // Call the callback function.
    CallFuncEx(this->LuaRefGetState(),
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
        "Identifier", cMember->IdentGet(), "Count", Refs);
  }
  /* -- Initialise new variables ------------------------------------------- */
  void LuaEvtInitEx(lua_State*const lS, const size_t stId)
  { // Check that the sent state is the same one we have stored
    if(this->LuaRefGetState() != lS)
      XC("Cannot use callback from a different state to the one currently "
         "stored as the internal storage only supports storing one state "
         "for multiple references!",
        "StoredState", this->LuaRefStateIsSet(), "SentState", lS != nullptr,
        "Identifier",  cMember->IdentGet(),         "Reference", stId,
        "Count",       Refs);
    // Initialise reference and if failed
    if(!this->LuaRefInitEx(stId))
      XC("Failed to reference new variable!",
        "Identifier", cMember->IdentGet(), "Reference", stId, "Count", Refs);
  }
  /* -- Initialise with empty function ------------------------------------- */
  void LuaEvtInit(lua_State*const lS)
  { // Need 2 parameters and a function
    CheckParams(lS, 2);
    CheckFunction(lS, 2, "Callback");
    // Done
    LuaEvtInitEx(lS);
  }
  /* -- Standby constructor ------------------------------------------------ */
  LuaEvtSlave(MemberType*const mtObject, const EvtMainCmd emcCmd) :
    /* -- Initialisers ----------------------------------------------------- */
    evtCmd(emcCmd),                    // Command event id
    cMember(mtObject)                  // Set pointer to member object
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(LuaEvtSlave);        // Disable copy constructor
};/* ----------------------------------------------------------------------- */
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */
