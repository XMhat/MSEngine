/* == EVTCORE.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This class allows threads to communicate with each other safely.    ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfEvtCore {                  // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfLog;                 // Using log namespace
/* -- Common events system (since we need to use this twice) --------------- */
template<typename Cmd,                 // Variable type of command to use
         size_t   EvtMaxEvents,        // Maximum number of events
         Cmd      EvtNone,             // Id of 'none' event
         Cmd      EvtNoLog>            // Id of succeeding ids to not log for
class EvtCore                          // Start of common event system class
{ /* -- Typedefs --------------------------------------------------- */ public:
  struct Cell;                         // (Prototype) Event packet info
  typedef void (CBFuncT)(const Cell&); // Event callback type
  typedef function<CBFuncT> CBFunc;    // Actual event callback
  /* ----------------------------------------------------------------------- */
  typedef array<CBFunc,EvtMaxEvents> Funcs;            // Reg'd events vector
  typedef list<Cell> Queue;                            // Current events queue
  typedef typename Queue::const_iterator QueueConstIt; // Current events queue
  /* ----------------------------------------------------------------------- */
  typedef const vector<pair<const Cmd,const CBFunc>> RegVec; // Event list
  /* ----------------------------------------------------------------------- */
  struct MVar                          // Multi-type helps access event data
  { /* --------------------------------------------------------------------- */
    union                              // Variables share same memory space
    { /* -- All these use the same memory ---------------------------------- */
      size_t             z;            // Size Integer ......... (4 or 8 bytes)
      bool               b;            // Boolean .................... (1 byte)
      void              *vp;           // Pointer .............. (4 or 8 bytes)
      float              f;            // Float ..................... (4 bytes)
      double             d;            // Double .................... (8 bytes)
      unsigned int       ui;           // Unsigned Integer .......... (4 bytes)
      signed int         i;            // Signed Integer ............ (4 bytes)
      unsigned long long ull;          // Unsigned Long Long ........ (8 bytes)
      signed long long   ll;           // Signed Long Long .......... (8 bytes)
      long unsigned int  lui;          // Long Unsigned Integer ..... (? bytes)
      long signed int    li;           // Long Signed Integer ....... (? bytes)
    }; /* ------------------------------------------------------------------ */
    explicit MVar(const void*const vpP): vp(const_cast<void*>(vpP)){}
    explicit MVar(const unsigned int uiV): ui(static_cast<unsigned int>(uiV)){}
    explicit MVar(const signed int siV): i(static_cast<signed int>(siV)){}
    explicit MVar(const double fdV): d(fdV){}
    explicit MVar(const float fV): f(fV){}
    explicit MVar(const unsigned long long ullV): ull(ullV){}
    explicit MVar(const signed long long sllV): ll(sllV){}
    explicit MVar(const long unsigned int luiV): lui(luiV){}
    explicit MVar(const long signed int liV): li(liV){}
    explicit MVar(const bool bV): b(bV){}
    /* --------------------------------------------------------------------- */
    MVar(void): ull(0){}
  };/* --------------------------------------------------------------------- */
  typedef vector<MVar> Params;         // vector of MVars Params;
  /* ----------------------------------------------------------------------- */
  struct Cell                          // Event packet information
  { /* --------------------------------------------------------------------- */
    Cmd            evtCommand;         // Command send
    CBFunc         evtcFunction;       // Function to call
    Params         vParams;            // User parameters
    /* -- Constructor with move parameters --------------------------------- */
    Cell(const Cmd evtC, const CBFunc &evtcF, Params &&vP) :
      /* -- Initialisers --------------------------------------------------- */
      evtCommand(evtC),                // Set requested command
      evtcFunction{ evtcF },           // Set callback function
      vParams{ std::move(vP) }              // Move requested parameters
      /* -- No code -------------------------------------------------------- */
      { }
    /* -- Initialiser with copy parameters --------------------------------- */
    Cell(const Cmd evtC, const CBFunc &evtcF, const Params &vP) :
      /* -- Initialisers --------------------------------------------------- */
      evtCommand(evtC),                // Set requested command
      evtcFunction{ evtcF },           // Set callback function
      vParams{ vP }                    // Copy requested parameters
      /* -- No code -------------------------------------------------------- */
      { }
    /* -- Move constructor ------------------------------------------------- */
    Cell(Cell &&cOther) :
      /* -- Initialisers --------------------------------------------------- */
      evtCommand(cOther.evtCommand),                  // Set other command
      evtcFunction{ std::move(cOther.evtcFunction) }, // Set other cb function
      vParams{ std::move(cOther.vParams) }            // Move other parameters
      /* -- No code -------------------------------------------------------- */
      { }
    /* --------------------------------------------------------------------- */
    DELETECOPYCTORS(Cell);             // No copying of this structure allowed
  };/* -- Private variables --------------------------------------- */ private:
  Funcs            evtData;            // Event callback storage
  mutex            mMutex;             // Primary events list mutex
  Queue            eEvents;            // Primary events list
  const string     strName;            // Name of event module
  /* -- Generic event ------------------------------------------------------ */
  void BlankFunction(const Cell &cData)
  { // Log the error
    cLog->LogWarningExSafe("$ ignored unregistered event! ($>$).",
      strName, cData.evtCommand, cData.vParams.size());
  }
  /* -- Get a function ----------------------------------------------------- */
  const CBFunc GetFunction(const Cmd eCmd)
  { // Get event function and return if it is valid
    const size_t stId = static_cast<size_t>(eCmd);
    if(stId < evtData.size()) return evtData[stId];
    // Log the error
    cLog->LogWarningExSafe("$ accessed an invalid event! ($>$).",
      strName, stId, evtData.size());
    // Return a blank function
    return bind(&EvtCore::BlankFunction, this, _1);
  }
  /* -- Execute specified event NOW (finisher) ----------------------------- */
  void ExecuteParam(const Cmd eCmd, Params &pData)
  { // Get callback function
    const CBFunc &fCB = GetFunction(eCmd);
    // Execute callback function
    fCB({ eCmd, fCB, std::move(pData) });
  }
  /* -- Execute specified event NOW (parameters) --------------------------- */
  template<typename... V,typename T>
    void ExecuteParam(const Cmd eCmd, Params &pData, T tParam, V... vVars)
  { // Insert parameter into list
    pData.push_back(MVar{ tParam });
    // Add more parameters
    ExecuteParam(eCmd, pData, vVars...);
  }
  /* -- Add with copy parameter semantics (finisher) ----------------------- */
  void AddParam(const Cmd eCmd, Params &pData)
  { // Event data to add to events list
    Cell cData{ eCmd, GetFunction(eCmd), std::move(pData) };
    // Wait and lock main event list
    const LockGuard lgEventsSync{ mMutex };
    // Move cell into event list
    eEvents.emplace_back(std::move(cData));
  }
  /* -- Add with copy parameter semantics (parameters) --------------------- */
  template<typename... V,typename T>
    void AddParam(const Cmd eCmd, Params &pData, T tParam, V... vVars)
  { // Place parameter in list
    pData.emplace_back(MVar{ tParam });
    // Add more parameters or finish
    AddParam(eCmd, pData, vVars...);
  }
  /* -- list is empty? --------------------------------------------- */ public:
  bool Empty(void)
  { // Lock access to events list
    const LockGuard lgEventsSync{ mMutex };
    // Return if queue is empty
    return eEvents.empty();
  }
  /* -- Returns number of events in queue ---------------------------------- */
  size_t SizeSafe(void)
  { // Lock access to events list
    const LockGuard lgEventsSync{ mMutex };
    // Return number of elements in queue
    return eEvents.size();
  }
  /* -- Returns the final iterator ----------------------------------------- */
  const QueueConstIt Last(void) { return eEvents.cend(); }
  /* -- Manage without lock ------------------------------------------------ */
  Cmd ManageUnsafe(void)
  { // Until event list is empty
    while(!eEvents.empty())
    { // Get event data. Move it and never reference it!
      const Cell epData{ std::move(eEvents.front()) };
      // Erase element. We're done with it. This is needed here incase the
      // callback throws an exception and causes an infinite loop.
      eEvents.pop_front();
      // Log event if loggable
      if(epData.evtCommand < EvtNoLog)
        cLog->LogDebugExSafe("$ processing event $.",
          strName, epData.evtCommand);
      // No callback? Return command to loop
      if(!epData.evtcFunction) return epData.evtCommand;
      // Execute the event callback
      epData.evtcFunction(epData);
    } // Return no significant event
    return EvtNone;
  }
  /* -- Manage with lock --------------------------------------------------- */
  Cmd ManageSafe(void)
  { // Try to lock access to events list.
    UniqueLock uLock{ mMutex, try_to_lock };
    // Since we call this in our engine loop. We are in a time critical
    // situation so we need to continue executing instead of waiting for
    // other threads to post events. So just return no event if threads are
    // busy posting events.
    if(uLock.owns_lock())
    { // Until event list is empty
      while(!eEvents.empty())
      { // Get event data. Move it and never reference it!
        const Cell epData{ std::move(eEvents.front()) };
        // Erase element. We're done with it. This is needed here incase the
        // callback throws an exception and causes an infinite loop.
        eEvents.pop_front();
        // Log event if loggable
        if(epData.evtCommand < EvtNoLog)
          cLog->LogDebugExSafe("$ system processing event $.",
            strName, epData.evtCommand);
        // No callback? Return command to loop
        if(!epData.evtcFunction) return epData.evtCommand;
        // Unlock mutex so events can still be added
        uLock.unlock();
        // Execute the event callback
        epData.evtcFunction(epData);
        // Relock mutex if we can and break out of loop if failed
        if(!uLock.try_lock()) break;
      }
    } // Return no significant event
    return EvtNone;
  }
  /* -- Flush events list -------------------------------------------------- */
  void Flush(void) { const LockGuard lgEventsSync{ mMutex }; eEvents.clear(); }
  /* -- Execute specified event NOW (starter) ------------------------------ */
  template<typename... V>void Execute(const Cmd eCmd, V... vVars)
  { // Parameters list
    Params pData;
    // Reserve memory for parameters
    pData.reserve(sizeof...(V));
    // Prepare parameters list and execute
    ExecuteParam(eCmd, pData, vVars...);
  }
  /* -- Add with copy parameter semantics (starter) ------------------------ */
  template<typename... V>void Add(const Cmd eCmd, V... vVars)
  { // Parameters list
    Params pData;
    // Reserve memory for parameters
    pData.reserve(sizeof...(V));
    // Prepare parameters list and add a new event
    AddParam(eCmd, pData, vVars...);
  }
  /* -- Add to events and return iterator (finisher) ----------------------- */
  void AddExParam(const Cmd eCmd, QueueConstIt &qciItem, Params &pData)
  { // Setup cell to insert
    Cell cData{ eCmd, GetFunction(eCmd), std::move(pData) };
    // Try to lock main event list
    const LockGuard lgEventsSync{ mMutex };
    // Push new event whilst move parameters into it
    qciItem = std::move(eEvents.emplace(eEvents.cend(), std::move(cData)));
  }
  /* -- Add to events and return iterator (parameters) --------------------- */
  template<typename... V,typename T>void AddExParam(const Cmd eCmd,
    QueueConstIt &qciItem, Params &pData, T tParam, V... vVars)
  { // Place parameter into parameter list
    pData.emplace_back(MVar{ tParam });
    // Add more parameters or finish
    AddExParam(eCmd, qciItem, pData, vVars...);
  }
  /* -- Queue and event and return the id of the event copy params --------- */
  template<typename... V>const QueueConstIt AddEx(const Cmd eCmd, V... vVars)
  { // Iterator to return
    QueueConstIt qciItem;
    // Parameters list
    Params pData;
    // Reserve parameters
    pData.reserve(sizeof...(V));
    // Prepare parameters list and execute
    AddExParam(eCmd, qciItem, pData, vVars...);
    // Return iterator
    return qciItem;
  }
  /* -- Remove event ------------------------------------------------------- */
  void Remove(const QueueConstIt &qIter)
  { // Try to lock main event list
    const LockGuard lgEventsSync{ mMutex };
    // Remove the event
    eEvents.erase(qIter);
  }
  /* -- Register event ----------------------------------------------------- */
  void Register(const Cmd eCmd, const CBFunc &ecFunc)
  { // Bail if invalid command
    if(static_cast<size_t>(eCmd) >= evtData.size())
      XC("Invalid registration command!",
         "System",   strName, "Event", eCmd,
         "Function", reinterpret_cast<const void*>(&ecFunc));
    // Assign callback function to event
    evtData[static_cast<size_t>(eCmd)] = ecFunc;
  }
  /* -- Register multiple events ------------------------------------------- */
  void RegisterEx(const RegVec &clCmds)
    { for(const auto &clP : clCmds) Register(clP.first, clP.second); }
  /* -- Unregister event --------------------------------------------------- */
  void Unregister(const Cmd eCmd)
  { // Bail if invalid command
    if(static_cast<size_t>(eCmd) >= evtData.size())
      XC("Invalid de-registration command!", "System", strName, "Event", eCmd);
    // Unassign callback function
    evtData[static_cast<size_t>(eCmd)] =
      bind(&EvtCore::BlankFunction, this, _1);
  }
  /* -- Unregister multiple events ----------------------------------------- */
  void UnregisterEx(const RegVec &clCmds)
    { for(const auto &clP : clCmds) Unregister(clP.first); }
    /* -- Event data, all empty functions ---------------------------------- */
  explicit EvtCore(const string &strN) : strName{std::move(strN)}
    { evtData.fill(bind(&EvtCore::BlankFunction, this, _1)); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(EvtCore);            // Delete copy constructor and operator
};/* ----------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */
