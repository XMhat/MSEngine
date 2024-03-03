/* == LUAVAR.HPP =========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Handles Lua CVars.                                                  ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ILuaVariable {               // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICollector::P;         using namespace ICVarDef::P;
using namespace ICVar::P;              using namespace ICVarLib::P;
using namespace IError::P;             using namespace IIdent::P;
using namespace ILuaUtil::P;           using namespace ILuaFunc::P;
using namespace ILua::P;               using namespace IString::P;
using namespace IStd::P;               using namespace ISysUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Lua cvar list types -------------------------------------------------- */
typedef pair<LuaFunc, CVarMapIt>         LuaCVarPair;
typedef pair<const string, LuaCVarPair>  LuaCVarMapPair;
typedef map<LuaCVarMapPair::first_type,
            LuaCVarMapPair::second_type> LuaCVarMap;
typedef LuaCVarMap::iterator             LuaCVarMapIt;
typedef LuaCVarMap::const_iterator       LuaCVarMapItConst;
/* -- Variables ollector class for collector data and custom variables ----- */
BEGIN_COLLECTOREX(Variables, Variable, CLHelperSafe,
  LuaCVarMap       lcvmMap;            /* Lua cvar list                     */\
);/* -- Lua variables collector and member class --------------------------- */
BEGIN_MEMBERCLASS(Variables, Variable, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Lockable                      // Lua garbage collector instruction
{ /* -- Private variables -------------------------------------------------- */
  LuaCVarMapIt     lcvmiIt;            // Iterator to command Console gives us
  /* -- Returns the lua console command list ------------------------------- */
  LuaCVarMap &GetLuaVarList(void) { return cVariables->lcvmMap; }
  /* -- Returns the end of the lua console command list -------------------- */
  LuaCVarMapIt GetLuaVarListEnd(void) { return GetLuaVarList().end(); }
  /* == Cvar updated callback for Lua ============================== */ public:
  static CVarReturn LuaCallbackStatic(
    CVarItem &cviVar,               // cppcheck-suppress constParameterCallback
    const string &strVal)
  { // Find cvar and ignore if we don't have it yet! This can happen if the
    // variable is initialising for the first time. We haven't added the
    // variable to cvmActive yet and we don't want to until the CVARS system
    // has created the variable.
    const LuaCVarMapIt lcvmpIt{ cVariables->lcvmMap.find(cviVar.GetVar()) };
    if(lcvmpIt == cVariables->lcvmMap.cend()) return ACCEPT;
    // Call the Lua callback assigned. We're expecting one return value
    lcvmpIt->second.first.LuaFuncProtectedDispatch(1, strVal, cviVar.GetVar());
    // Get result of the callback which means a boolean HAS to be returned or
    // we will deny the cvar change by default.
    const CVarReturn cvResult = BoolToCVarReturn(
      lua_isboolean(cLuaFuncs->LuaRefGetState(), -1) &&
      lua_toboolean(cLuaFuncs->LuaRefGetState(), -1));
    // Remove whatever it was that was returned by the callback
    LuaUtilRmStack(cLuaFuncs->LuaRefGetState());
    // Return result to cvar set function
    return cvResult;
  }
  /* -- Unregister the console command from lua -------------------- */ public:
  const string &Name(void) const { return lcvmiIt->first; }
  /* -- Get current value as string ---------------------------------------- */
  const string Get(void) const
    { return cCVars->GetStr(lcvmiIt->second.second); }
  /* -- Get default value as string ---------------------------------------- */
  const string Default(void) const
    { return cCVars->GetDefStr(lcvmiIt->second.second); }
  /* -- Reset default value ------------------------------------------------ */
  void Reset(void) const { cCVars->Reset(lcvmiIt->second.second); }
  /* -- Returns if value is empty ------------------------------------------ */
  bool Empty(void) const { return Get().empty(); }
  /* -- Set value ---------------------------------------------------------- */
  CVarSetEnums Set(const string &strValue) const
    { return cCVars->Set(lcvmiIt->second.second, strValue); }
  /* -- Register user console command from lua ----------------------------- */
  void Init(lua_State*const lS)
  { // Must be running on the main thread
    cLua->StateAssert(lS);
    // Must have 5 parameters (including this class ptr that was just created)
    LuaUtilCheckParams(lS, 5);
    // Get and check the variable name
    const string strName{ LuaUtilGetCppStr(lS, 1, "Variable") };
    // Check that the variable name is valid
    if(!cCVars->IsValidVariableName(strName))
      XC("CVar name is not valid!",
         "Variable", strName,
         "Minimum",  cCVars->stCVarMinLength,
         "Maximum",  cCVars->stCVarMaxLength);
    // Make sure cvar doesn't already exist
    if(cCVars->VarExists(strName))
      XC("CVar already registered!", "Variable", strName);
    // Get the value name
    const string strD{ LuaUtilGetCppStr(lS, 2, "Default") };
    // Get the flags and check that the flags are in range
    const CVarFlagsConst cvfcFlags{ LuaUtilGetFlags(lS, 3, CVMASK, "Flags") };
    // Check that the var has at least one type
    if(!(cvfcFlags.FlagIsAnyOfSet(TSTRING|TINTEGER|TFLOAT|TBOOLEAN) &&
      // Check that types are not mixed
      cvfcFlags.FlagIsAnyOfSetAndClear(
        TSTRING,  /* <- Set? & Clear? -> */ TINTEGER|TFLOAT|TBOOLEAN,  /* Or */
        TINTEGER, /* <- Set? & Clear? -> */ TSTRING|TFLOAT|TBOOLEAN,   /* Or */
        TFLOAT,   /* <- Set? & Clear? -> */ TSTRING|TINTEGER|TBOOLEAN, /* Or */
        TBOOLEAN, /* <- Set? & Clear? -> */ TSTRING|TINTEGER|TFLOAT)))
      XC("CVar flags have none or mixed types!",
         "Variable", strName, "Flags", cvfcFlags.FlagGet());
    // Check that the fourth parameter is a function
    LuaUtilCheckFunc(lS, 4, "Callback");
    // Since the userdata for this class object is at arg 5, we need to make
    // sure the callback function is ahead of it in arg 6 or the LuaFunc()
    // class which calls luaL_ref will fail as it ONLY reads position -1.
    lua_pushvalue(lS, 4);
    // Save the function at the top of the stack used for the callback
    lcvmiIt = cVariables->lcvmMap.insert(GetLuaVarListEnd(), { strName,
      make_pair(LuaFunc{ StrAppend("CV:", strName), true },
        cCVars->GetVarListEnd())
    }); // Register the variable and set the iterator to the new cvar.
    lcvmiIt->second.second = cCVars->RegisterVar(strName, strD,
      LuaCallbackStatic, cvfcFlags|TLUA|PANY);
  }
  /* -- Destructor that unregisters the cvar ------------------------------- */
  ~Variable(void)
  { // Return if the iterator is invalid?
    if(lcvmiIt == GetLuaVarListEnd()) return;
    // Unregister the cvar
    if(lcvmiIt->second.second != cCVars->GetVarListEnd())
      cCVars->UnregisterVar(lcvmiIt->second.second);
    // Remove the lua var
    GetLuaVarList().erase(lcvmiIt);
  }
  /* -- Basic constructor with no init ------------------------------------- */
  Variable(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperVariable{                  // Initialise and register the object
      cVariables, this },
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    lcvmiIt{ GetLuaVarListEnd() }      // Initialise iterator to the last
    /* --------------------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Variable)            // Disable copy constructor and operator
};/* ----------------------------------------------------------------------- */
END_COLLECTOR(Variables)               // Finish global Files collector
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
