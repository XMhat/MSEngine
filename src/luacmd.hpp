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
using namespace ICollector::P;         using namespace IConsole::P;
using namespace IIdent::P;             using namespace ILuaUtil::P;
using namespace ILuaFunc::P;           using namespace ILua::P;
using namespace IStd::P;               using namespace ISysUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- File collector and member class -------------------------------------- */
BEGIN_COLLECTORDUO(Commands, Command, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Lockable                      // Lua garbage collector instruction
{ /* -- Private variables -------------------------------------------------- */
  LuaFunc::MapIt lfmiIterator;         // Iterator to command Console gives us
  /* -- Unregister the console command from lua -------------------- */ public:
  const string &Name(void) const { return lfmiIterator->first; }
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
    // Register the console command
    lfmiIterator = cConsole->RegisterLuaCommand(strName, uiMinimum, uiMaximum);
  }
  /* -- Destructor that unregisters the cvar ------------------------------- */
  ~Command(void) { cConsole->UnregisterLuaCommand(lfmiIterator); }
  /* -- Basic constructor with no init ----------------------------- */ public:
  Command(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperCommand{                   // Initialise and register the object
      cCommands, this },
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    lfmiIterator{ cConsole->           // Initialise iterator to the last...
      GetLuaConCmdListEnd() }          // ...Lua console command in the map
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
