/* == LUAREF.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This class manages function callbacks for certain core engine       ## */
/* ## events which should only be used witrh the main Lua thread only.    ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfLuaFunc {                   // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfLuaUtil;             // Using luautil namespace
using namespace IfLog;                 // Using log namespace
using namespace IfLuaRef;              // Using luaref namespace
using namespace IfCollector;           // Using collector namespace
/* -- LuaFunc ollector class for collector data and custom variables ------- */
BEGIN_COLLECTOREX(LuaFuncs, LuaFunc, CLHelperSafe, /**/,/**/, public LuaRef<1>)
/* -- LuaFunc class -------------------------------------------------------- */
BEGIN_MEMBERCLASS(LuaFuncs, LuaFunc, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Ident                         // Object name
{ /* -- Private variables -------------------------------------------------- */
  int              iRef;               // The reference
  int              iRefS;              // The reference (when lua is paused)
  /* -- Public typedefs -------------------------------------------- */ public:
  typedef map<const string, LuaFunc> Map;        // Map for cvars/console
  typedef Map::iterator              MapIt;      // Map iterator
  typedef Map::const_iterator        MapItConst; // Map const iterator
  /* -- Public functions --------------------------------------------------- */
  void LuaFuncSwap(LuaFunc &oCref)
  { // Convert to non-const and swap members
    swap(iRef, oCref.iRef);
    swap(iRefS, oCref.iRefS);
    IdentSwap(oCref);
    // Swap registration
    CollectorSwapRegistration(oCref);
  }
  /* -- Restore reference and reset saved reference if it is not set ------- */
  void LuaFuncEnable(void)
    { if(iRefS != LUA_REFNIL) { iRef = iRefS; iRefS = LUA_REFNIL; } }
  /* -- Save reference and set main reference to empty function if set ----- */
  void LuaFuncDisable(void)
    { if(iRefS == LUA_REFNIL) { iRefS = iRef; iRef = cParent.LuaRefGetId(); } }
  /* -- Returns the reference to this function ----------------------------- */
  int LuaFuncGet(void) const { return iRef; }
  /* -- Returns the saved reference to this function ----------------------- */
  int LuaFuncGetSaved(void) const { return iRefS; }
  /* -- Check to see if we can add the specified number of parameters ------ */
  bool LuaFuncCheckAddParams(const size_t stParams, const char*const cpType)
  { // Return if value is valid
    if(StackHasCapacity(cParent.LuaRefGetState(), stParams)) return true;
    // Write warning to log
    cLog->LogWarningExSafe("LuaFunc cannot add $ more $ parameters for "
      "calling '$' due to integer or potential stack overflow!",
      stParams, cpType, IdentGet());
    // Failed
    return false;
  }
  /* -- Send nothing ------------------------------------------------------- */
  void LuaFuncParams(int&) { }
  /* -- Send string vector ------------------------------------------------- */
  template<typename... Vars>
    void LuaFuncParams(int &iParams, const StrVector &svList,
      const Vars... vVars)
  { // If we have items
    if(!svList.empty())
    { // Make sure the number of parameters would not overflow
      if(!LuaFuncCheckAddParams(svList.size(), "vector strings")) return;
      // Convert to table on stack
      for(const string &strStr : svList)
        PushCppString(cParent.LuaRefGetState(), strStr);
      // Increase number of parameters
      iParams += static_cast<int>(svList.size());
    } // Next item
    LuaFuncParams(iParams, vVars...);
  }
  /* ----------------------------------------------------------------------- */
  template<typename... Vars>
    void LuaFuncParams(int &iParams, const string &strVal,
      const Vars... vVars)
  { // Make sure the number of parameters would not overflow
    if(!LuaFuncCheckAddParams(1, "string")) return;
    // Copy string to stack
    PushCppString(cParent.LuaRefGetState(), strVal);
    // Next item
    LuaFuncParams(++iParams, vVars...);
  }
  /* -- Helper function to make LUAREFDISPATCH parameters ------------------ */
#define MP(t,s,f) \
  template<typename... Vars> \
    void LuaFunc ## Params(int &iParams, const t tValue, \
      const Vars... vVars) \
  { \
    if(!LuaFuncCheckAddParams(1, s)) return; \
    f(cParent.LuaRefGetState(), tValue); \
    LuaFunc ## Params(++iParams, vVars...); \
  }
  /* -- A function for each type ------------------------------------------- */
  MP(signed long long,   "int64",  PushInteger)
  MP(unsigned long long, "uint64", PushInteger)
  MP(signed int,         "int",    PushInteger)
  MP(unsigned int,       "uint",   PushInteger)
  MP(float,              "float",  PushNumber)
  MP(double,             "double", PushNumber)
  MP(bool,               "bool",   PushBoolean)
  /* -- Done with helper function ------------------------------------------ */
#undef MP
  /* -- Send a function ---------------------------------------------------- */
  void LuaFuncPushFunc(void) const
  { // Get referenced function and return if succeeded
    if(GetReferencedFunction(cParent.LuaRefGetState(), iRef)) return;
    // Failed so throw error
    XC("Pushed function is not a valid function!",
       "Name",  IdentGet(), "Value", iRef,
       "Stack", GetVarStack(cParent.LuaRefGetState()));
  }
  /* -- Dispatch the requested variables ----------------------------------- */
  template<typename... Vars>void LuaFuncDispatch(const Vars... vArgs)
  { // Push the call back function
    LuaFuncPushFunc();
    // Number of parameters written. This cannot be optimised with sizeof...()
    // because we might want to add multiple entries from a list.
    int iParams = 0;
    // Push all the parameters
    LuaFuncParams(iParams, vArgs...);
    // Do the call
    CallFuncEx(cParent.LuaRefGetState(), iParams);
  }
  /* -- Dispatch the requested variables safely ---------------------------- */
  template<typename... Vars>
    void LuaFuncProtectedDispatch(const int iReturns, const Vars... vArgs)
  { // Save stack position so we can restore it on error
    const int iStack = GetStackCount(cParent.LuaRefGetState()),
    // Push generic error function. This needs to be cleaned up after PCall
    iErrorCallback = PushAndGetGenericErrorId(cParent.LuaRefGetState());
    // This exception block is so we can remove the error callback
    try
    { // Push the call back function
      LuaFuncPushFunc();
      // Number of parameters written
      int iParams = 0;
      // Push all the parameters sent by the caller
      LuaFuncParams(iParams, vArgs...);
      // Do the secure call. The exception handler is to cleam
      PCallSafe(cParent.LuaRefGetState(), iParams, iReturns, iErrorCallback);
    } // Exception occured?
    catch(const exception&)
    { // Restore stack position because we don't know what might have added
      PruneStack(cParent.LuaRefGetState(), iStack);
      // Rethrow the error
      throw;
    }
  }
  /* -- Push function and call it ------------------------------------------ */
  void LuaFuncPushAndCall(void) const
  { // Push function onto stack
    LuaFuncPushFunc();
    // Do the call with no params nor returns
    CallFunc(cParent.LuaRefGetState());
  }
  /* -- De-initialise saved function --------------------------------------- */
  void LuaFuncDeInit(void)
  { // Reset user set function if it isn't the empty function
    if(iRef != cParent.LuaRefGetId())
      SafeDeleteReference(cParent.LuaRefGetState(), iRef,
        cParent.LuaRefGetId());
    // Reset saved function from pause request if it isn't the empty function
    if(iRefS != cParent.LuaRefGetId())
      SafeDeleteReference(cParent.LuaRefGetState(), iRefS);
  }
  /* -- Set a new function ------------------------------------------------- */
  void LuaFuncSet(void)
  { // If last item on stack is a C function?
    if(lua_iscfunction(cParent.LuaRefGetState(), -1))
    { // De-init old reference if it not empty function
      if(iRef != cParent.LuaRefGetId() && iRef != LUA_REFNIL &&
        !SafeDeleteReference(cParent.LuaRefGetState(), iRef,
          cParent.LuaRefGetId()))
            cLog->LogErrorSafe(
              "LuaFunc couldn't delete old ref for C function!");
      // Set reference to C function
      if(!InitReference(cParent.LuaRefGetState(), iRef))
        XC("Failed to create refid to C function!",
           "Name",  IdentGet(), "Value", iRef,
           "Stack", GetVarStack(cParent.LuaRefGetState()));
      // Succeeded so put in log
      cLog->LogDebugExSafe("LuaFunc allocated refid #$ for C function '$'.",
        iRef, IdentGet());
    } // If last item on stack is a regular function?
    else if(lua_isfunction(cParent.LuaRefGetState(), -1))
    { // Set reference to c function. Do NOT de-initialise empty function
      if(iRef != cParent.LuaRefGetId() && iRef != LUA_REFNIL &&
        !SafeDeleteReference(cParent.LuaRefGetState(),
          iRef, cParent.LuaRefGetId()))
            cLog->LogErrorExSafe(
              "LuaFunc couldn't delete old ref for function!");
      // Set reference to regular function
      if(!InitReference(cParent.LuaRefGetState(), iRef))
        XC("Failed to create refid to function!",
           "Name",  IdentGet(), "Value", iRef,
           "Stack", GetVarStack(cParent.LuaRefGetState()));
      // Succeeded so fut in log
      cLog->LogDebugExSafe("LuaFunc allocated refid #$ for function '$'.",
        iRef, IdentGet());
    } // Don't know what this was?
    else XC("Expected C or regular function type on stack!",
            "Name",  IdentGet(),
            "Type",  lua_typename(cParent.LuaRefGetState(), -1),
            "Stack", GetVarStack(cParent.LuaRefGetState()));
  }
  /* -- Set empty function ------------------------------------------------- */
  void LuaFuncSetEmptyFunc(void) { iRef = cParent.LuaRefGetId(); }
  /* -- Move constructor --------------------------------------------------- */
  LuaFunc(LuaFunc &&lfOther) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperLuaFunc{ *cLuaFuncs,       // Set name in collector
      this },                          // Supply parent class to collector
    IdentCSlave{ cParent.CtrNext() },  // Initialise identification number
    Ident{ StdMove(lfOther) },          // Move name
    iRef(lfOther.iRef),                // Copy other reference
    iRefS(lfOther.iRefS)               // Copy other saved reference
    /* -- Code ------------------------------------------------------------- */
    { // Clear the other references
      lfOther.iRef = lfOther.iRefS = LUA_REFNIL;
      // Unregister other from collector
      lfOther.CollectorUnregister();
    }
  /* -- Disabled constructor without registration -------------------------- */
  explicit LuaFunc(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperLuaFunc{ *cLuaFuncs },     // Set parent for collector
    IdentCSlave{ cParent.CtrNext() },  // Initialise identification number
    iRef(LUA_REFNIL),                  // Reference not initialised
    iRefS(LUA_REFNIL)                  // Saved reference not initialised
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Name constructor --------------------------------------------------- */
  explicit LuaFunc(const string &strN, const bool bSet=false) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperLuaFunc{                   // Register in collector
      *cLuaFuncs, this },              // Collector and this object
    IdentCSlave{ cParent.CtrNext() },  // Initialise identification number
    Ident{ strN },                     // Copy name
    iRef(LUA_REFNIL),                  // Reference not initialised
    iRefS(LUA_REFNIL)                  // Saved reference not initialised
    /* -- Set if requested ------------------------------------------------- */
    { if(bSet) LuaFuncSet(); }
  /* -- Name(move) constructor --------------------------------------------- */
  explicit LuaFunc(string &&strN, const bool bSet=false) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperLuaFunc{                   // Register in collector
      *cLuaFuncs, this },              // Collector and this object
    IdentCSlave{ cParent.CtrNext() },  // Initialise identification number
    Ident{ StdMove(strN) },             // Move name
    iRef(LUA_REFNIL),                  // Reference not initialised
    iRefS(LUA_REFNIL)                  // Saved reference not initialised
    /* -- Set if requested ------------------------------------------------- */
    { if(bSet) LuaFuncSet(); }
  /* ----------------------------------------------------------------------- */
  ~LuaFunc(void) { LuaFuncDeInit(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(LuaFunc)              // Disable copy constructor/operator
};/* ----------------------------------------------------------------------- */
/* -- De-init state and all references ------------------------------------- */
static void LuaFuncDeInitRef(void)
{ // Ignore if no state or function
  if(!cLuaFuncs->LuaRefStateIsSet()) return;
  // Write to log that we're deinitialising
  cLog->LogDebugSafe("LuaFuncs de-initialising...");
  // If there are references?
  if(!cLuaFuncs->empty())
  { // DeInit all references
     cLog->LogDebugExSafe("LuaFuncs de-initialising $ engine event refids...",
       cLuaFuncs->size());
     for(LuaFunc*const lCptr : *cLuaFuncs) lCptr->LuaFuncDeInit();
     cLog->LogInfoExSafe("LuaFuncs de-initialised $ engine event refids.",
       cLuaFuncs->size());
  } // De-initialise the empty function and log the result
  cLog->LogDebugSafe(cLuaFuncs->LuaRefDeInit() ?
    "LuaFuncs de-initialised empty function refid." :
    "LuaFuncs failed to de-init empty function refid.");
  // Write to log that we're deinitialising
  cLog->LogDebugSafe("LuaFuncs de-initialised.");
}
/* -- Empty function ------------------------------------------------------- */
static int LuaFuncEmptyCFunction(lua_State*const) { return 0; }
/* -- Init with lua state -------------------------------------------------- */
static void LuaFuncInitRef(lua_State*const lS)
{ // DeInit current lua refs
  LuaFuncDeInitRef();
  // Write to log that we're deinitialising
  cLog->LogDebugSafe("LuaFuncs manager initialising...");
  // Push empty function and reference it into the lua stack
  PushCFunction(lS, LuaFuncEmptyCFunction);
  cLuaFuncs->LuaRefInit(lS);
  // Log result
  cLog->LogDebugExSafe("LuaFunc allocated refid #$ for empty function.",
    cLuaFuncs->LuaRefGetId());
   // If there are references?
  if(!cLuaFuncs->empty())
  { // Init all references to the empty function
     cLog->LogDebugExSafe("LuaFuncs initialising $ engine event refids...",
       cLuaFuncs->size());
     for(LuaFunc*const lCptr : *cLuaFuncs) lCptr->LuaFuncSetEmptyFunc();
     cLog->LogDebugExSafe("LuaFuncs initialised $ engine event refids.",
       cLuaFuncs->size());
  } // Write to log that we're deinitialising
  cLog->LogDebugSafe("LuaFuncs manager initialised!");
}
/* -- Disable all references and add to errors if it didn't work ----------- */
static void LuaFuncDisableAllRefs(void)
{ // Return if no refs to disable
  if(cLuaFuncs->empty()) return;
  // Disable all references and add to errors if it didn't work
  cLog->LogDebugExSafe("LuaFuncs disabling $ references...",
    cLuaFuncs->size());
  for(LuaFunc*const lCptr : *cLuaFuncs) lCptr->LuaFuncDisable();
  cLog->LogDebugExSafe("LuaFuncs disabled $ references...", cLuaFuncs->size());
}
/* -- Mass enable all references ------------------------------------------- */
static void LuaFuncEnableAllRefs(void)
{ // Return if no refs to enable
  if(cLuaFuncs->empty()) return;
  // Enable all references and add to errors if it didn't work
  cLog->LogDebugExSafe("LuaFuncs enabling $ references...", cLuaFuncs->size());
  for(LuaFunc*const lCptr : *cLuaFuncs) lCptr->LuaFuncEnable();
  cLog->LogDebugExSafe("LuaFuncs enabled $ references...", cLuaFuncs->size());
}
/* ------------------------------------------------------------------------- */
END_COLLECTOREX(LuaFuncs,,LuaFuncDeInitRef(),) // Finish collector
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */
