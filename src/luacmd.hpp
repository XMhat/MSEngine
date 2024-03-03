/* == LUACMD.HPP =========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Handles Lua Commands.                                               ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ILuaCommand {                // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IArgs;                 using namespace ICollector::P;
using namespace IConsole::P;           using namespace IError::P;
using namespace IIdent::P;             using namespace ILog::P;
using namespace ILuaUtil::P;           using namespace ILuaFunc::P;
using namespace ILua::P;               using namespace IStd::P;
using namespace IString::P;            using namespace ISysUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
typedef pair<LuaFunc, CmdMapIt>        LuaCmdPair;       // Lua id/cvar list
typedef map<const string, LuaCmdPair>  LuaCmdMap;        // Map for lua vars
typedef LuaCmdMap::iterator            LuaCmdMapIt;      // Map iterator
typedef LuaCmdMap::const_iterator      LuaCmdMapItConst; // Map const it
/* -- Variables ollector class for collector data and custom variables ----- */
BEGIN_COLLECTOREX(Commands, Command, CLHelperSafe,
  LuaCmdMap        lcmMap;             /* Lua console command list          */\
);/* -- Lua command collector and member class ----------------------------- */
BEGIN_MEMBERCLASS(Commands, Command, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Lockable                      // Lua garbage collector instruction
{ /* -- Private variables -------------------------------------------------- */
  LuaCmdMapIt      lcmiIt;             // Iterator to command Console gives us
  /* -- Returns the lua command list --------------------------------------- */
  LuaCmdMap &GetLuaCmdsList(void) { return cParent->lcmMap; }
  /* -- Returns the end of the lua command list ---------------------------- */
  LuaCmdMapIt GetLuaCmdsListEnd(void) { return GetLuaCmdsList().end(); }
  /* -- Push and get error callback function id ---------------------------- */
  static void LuaCallbackStatic(const Args &aArgs)
  { // Find command in console command list and log if not found (impossible)
    const LuaCmdMapIt lcmiIt{ cCommands->lcmMap.find(aArgs.front()) };
    if(lcmiIt == cCommands->lcmMap.end())
      cLog->LogWarningExSafe("Command can't find virtual command '$'!",
        aArgs.front());
    // Call the function callback in Lua
    lcmiIt->second.first.LuaFuncProtectedDispatch(0, aArgs);
  }
  /* -- Unregister the console command from lua -------------------- */ public:
  const string &Name(void) const { return lcmiIt->first; }
  /* -- Register user console command from lua ----------------------------- */
  void Init(lua_State*const lS)
  { // Must be running on the main thread
    cLua->StateAssert(lS);
    // Must have 5 parameters (including the class from llconcmd.hpp)
    LuaUtilCheckParams(lS, 5);
    // Get command name, min and max parameter count. It's not const because
    // RegisterLuaCommand will do a StdMove() on it.
    string strName{ LuaUtilGetCppStrNE(lS, 1, "Name") };
    const unsigned int
      uiMinimum = LuaUtilGetInt<unsigned int>(lS, 2, "Minimum"),
      uiMaximum = LuaUtilGetInt<unsigned int>(lS, 3, "Maximum");
    // Check that the fourth parameter is a function
    LuaUtilCheckFunc(lS, 4, "Callback");
    // Since the userdata for this class object is at arg 5, we need to make
    // sure the callback function is ahead of it in arg 6 or the LuaFunc()
    // class which calls luaL_ref will fail as it ONLY reads position -1.
    lua_pushvalue(lS, 4);
    // Find command and throw exception if already exists
    if(GetLuaCmdsList().contains(strName))
      XC("Virtual command already exists!", "Command", strName);
    // Register the variable and get the iterator to the new cvar. Don't
    // forget the lua reference needs to be in place for when the callback
    // is called. Create a function and reference the function on the lua
    // stack and insert the reference into the list
    lcmiIt = GetLuaCmdsList().insert(GetLuaCmdsListEnd(), { StdMove(strName),
      make_pair(LuaFunc{ StrAppend("CC:", strName), true },
        cConsole->RegisterCommand(strName,
          uiMinimum, uiMaximum, LuaCallbackStatic)) });
  }
  /* -- Destructor that unregisters the cvar ------------------------------- */
  ~Command(void)
  { // Return if iterator is not registered
    if(lcmiIt == GetLuaCmdsListEnd()) return;
    // Unregister the command if set
    if(lcmiIt->second.second != cConsole->GetCmdsListEnd())
      cConsole->UnregisterCommand(lcmiIt->second.second);
    // Erase the item from the list
    GetLuaCmdsList().erase(lcmiIt);
  }
  /* -- Basic constructor with no init ----------------------------- */ public:
  Command(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperCommand{                   // Initialise and register the object
      cCommands, this },
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    lcmiIt{ GetLuaCmdsListEnd() }      // Initialise iterator to the last
    /* --------------------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Command)             // Disable copy constructor and operator
};/* ----------------------------------------------------------------------- */
END_COLLECTOR(Commands)                // Finish global Files collector
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
