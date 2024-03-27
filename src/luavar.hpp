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
using namespace ICVar::P;              using namespace IError::P;
using namespace IIdent::P;             using namespace ILuaUtil::P;
using namespace ILuaFunc::P;           using namespace ILua::P;
using namespace IStd::P;               using namespace ISysUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- File collector and member class -------------------------------------- */
BEGIN_COLLECTORDUO(Variables, Variable, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Lockable                      // Lua garbage collector instruction
{ /* -- Private variables -------------------------------------------------- */
  LuaCVarMapIt     lcvmiIt;            // Iterator to command Console gives us
  /* -- Unregister the console command from lua -------------------- */ public:
  const string &Name(void) const { return lcvmiIt->first; }
  /* -- Get current value as string ---------------------------------------- */
  const string Get(void) const { return cCVars->GetStrSafe(lcvmiIt); }
  /* -- Get default value as string ---------------------------------------- */
  const string Default(void) const { return cCVars->GetDefStrSafe(lcvmiIt); }
  /* -- Returns if value is empty ------------------------------------------ */
  bool Empty(void) const { return cCVars->IsVarStrEmptySafe(lcvmiIt); }
  /* -- Set value ---------------------------------------------------------- */
  CVarSetEnums Set(const string &strValue) const
    { return cCVars->SetSafe(lcvmiIt, strValue); }
  /* -- Register user console command from lua ----------------------------- */
  void Init(lua_State*const lS)
  { // Must be running on the main thread
    cLua->StateAssert(lS);
    // Must have 5 parameters (including this class ptr that was just created)
    LuaUtilCheckParams(lS, 5);
    // Get and check the variable name
    const string strName{ LuaUtilGetCppStr(lS, 1, "Variable") };
    // Check that the variable name is valid
    cCVars->CheckValidVariableName(strName);
    // Get the value name
    const string strD{ LuaUtilGetCppStr(lS, 2, "Default") };
    // Get the flags and check that the flags are in range
    const CVarFlagsConst cF{ LuaUtilGetFlags(lS, 3, CVMASK, "Flags") };
    // Check that the var has at least one type
    cCVars->CheckTypeValidInFlags(strName, cF);
    // Check that the fourth parameter is a function
    LuaUtilCheckFunc(lS, 4, "Callback");
    // Since the userdata for this class object is at arg 5, we need to make
    // sure the callback function is ahead of it in arg 6 or the LuaFunc()
    // class which calls luaL_ref will fail as it ONLY reads position -1.
    lua_pushvalue(lS, 4);
    // Register the console command
    lcvmiIt = cCVars->RegisterLuaVar(strName, strD, cF);
  }
  /* -- Destructor that unregisters the cvar ------------------------------- */
  ~Variable(void) { cCVars->UnregisterLuaVar(lcvmiIt); }
  /* -- Basic constructor with no init ------------------------------------- */
  Variable(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperVariable{                  // Initialise and register the object
      cVariables, this },
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    lcvmiIt{ cCVars->                  // Initialise iterator to the last...
      GetLuaVarListEnd() }             // ...Lua console command in the map
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
