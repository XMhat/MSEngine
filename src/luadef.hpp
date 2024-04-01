/* == LUADEF.HPP =========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Some definitions specific to Lua.                                   ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* == LuaDef interface namespace =========================================== */
namespace ILuaDef {                    // Start of module namespace
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
};                                     // End of module namespace
/* == LibLua interface namespace =========================================== */
namespace ILuaLib {                    // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace ICVarDef::P;           using namespace ILuaDef;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Variables ============================================================ */
struct LuaLibStatic                    // Name of struct
{ /* ----------------------------------------------------------------------- */
  const string_view   &strvName;       // Name of library
  const CoreFlagsConst cfcRequired;    // Required core flags to register
  const luaL_Reg*const libList;        // Library functions
  const int            iLLCount;       // Size of library functions
  const luaL_Reg*const libmfList;      // Member library functions
  const int            iLLMFCount;     // Size of member library functions
  const lua_CFunction  lcfpDestroy;    // Destruction function
  const LuaTable*const lkiList;        // Table of key/values to define
  const int            iLLKICount;     // Size of member library functions
  /* --------------------------------------------------------------------- */
};                                     // Library list in lualib.hpp
/* ------------------------------------------------------------------------- */
typedef array<const LuaLibStatic, 32> LuaLibStaticArray;
extern const LuaLibStaticArray luaLibList;
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
