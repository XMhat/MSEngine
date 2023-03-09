/* == LUADEF.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Some definitions specific to Lua.                                   ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfLuaDef {                   // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfCVarDef;             // Using util namespace
/* == Typedefs ============================================================= */
struct LuaKeyInt                       // Lua key/value pairs C
{ /* ----------------------------------------------------------------------- */
  const char*const  cpName;            // Name of const table
  const lua_Integer liValue;           // Integer value for this const
};/* ----------------------------------------------------------------------- */
struct LuaTable                        // Lua table as C
{ /* ----------------------------------------------------------------------- */
  const char*const      cpName;        // Name of const table
  const LuaKeyInt*const kiList;        // Key value list
  const int             iCount;        // Number of items in this list
};/* ----------------------------------------------------------------------- */
};                                     // End of luadef interface
/* == LibLua interface namespace =========================================== */
namespace IfLuaLib {                   // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfLuaDef;              // Using cvardef namespace
/* == Variables ============================================================ */
extern const struct LuaLibStatic       // Name of struct
{ /* ----------------------------------------------------------------------- */
  const char*const     cpName;         // Name of library
  const CoreFlagsConst cfRequired;     // Required core flags to register
  const luaL_Reg*const libList;        // Library functions
  const int            iLLCount;       // Size of library functions
  const luaL_Reg*const libmfList;      // Member library functions
  const int            iLLMFCount;     // Size of member library functions
  const lua_CFunction  lcfpDestroy;    // Destruction function
  const LuaTable*const lkiList;        // Table of key/values to define
  const int            iLLKICount;     // Size of member library functions
  /* --------------------------------------------------------------------- */
} luaLibList[];                        // Library list in lualib.hpp
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */
