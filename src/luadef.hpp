/* == LUADEF.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Some definitions specific to Lua.                                   ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfLuaDef {                   // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfCVarDef;             // Using util interface
/* == Typedefs ============================================================= */
struct LuaKeyInt                       // Lua key/value pairs C
{ /* ----------------------------------------------------------------------- */
  const char*const      cpName;        // Name of const table
  const lua_Integer     liValue;       // Integer value for this const
};/* ----------------------------------------------------------------------- */
struct LuaTable                        // Lua table as C
{ /* ----------------------------------------------------------------------- */
  const char*const      cpName;        // Name of const table
  const LuaKeyInt*const kiList;        // Key value list
  const int             iCount;        // Number of items in this list
};/* ----------------------------------------------------------------------- */
};                                     // End of luadef interface
/* == LibLua interface namespace =========================================== */
namespace IfLuaLib {                   // Beginning of lualib interface
/* -- Includes ------------------------------------------------------------- */
using namespace IfLuaDef;              // Using cvardef interface
/* == Variables ============================================================ */
extern const struct LuaLibStatic       // Name of struct
{ /* ----------------------------------------------------------------------- */
  const char*const      cpName;        // Name of library
  const GuiMode         guimMin;       // Minimum gui mode required
  const luaL_Reg*const  libList;       // Library functions
  const int             iLLCount;      // Size of library functions
  const luaL_Reg*const  libmfList;     // Member library functions
  const int             iLLMFCount;    // Size of member library functions
  const lua_CFunction   lcfpDestroy;   // Destruction function
  const LuaTable*const  lkiList;       // Table of key/values to define
  const int             iLLKICount;    // Size of member library functions
  /* --------------------------------------------------------------------- */
} luaLibList[];                        // Library list in lualib.hpp
/* ------------------------------------------------------------------------- */
};                                     // End of lualib interface
/* == EoF =========================================================== EoF == */
